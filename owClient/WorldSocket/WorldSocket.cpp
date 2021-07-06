#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WorldSocket.h"

// Additional
#include <zlib/source/zlib.h>

#include "OpcodesNames.h"
#include "AuthSocket/Cryptography/SHA1.h"


CWorldSocket::CWorldSocket(const std::string& Login, BigNumber Key)
	: m_CurrentPacket(nullptr)
	, m_Login(Login)
	, m_Key(Key)
{
	m_Warden = std::make_unique<CWarden>(*this, m_Key);

	AddHandler(SMSG_AUTH_CHALLENGE, std::bind(&CWorldSocket::On_SMSG_AUTH_CHALLENGE, this, std::placeholders::_1));
	AddHandler(SMSG_AUTH_RESPONSE,  std::bind(&CWorldSocket::On_SMSG_AUTH_RESPONSE, this, std::placeholders::_1));
	AddHandler(SMSG_WARDEN_DATA,    std::bind(&CWorldSocket::On_SMSG_WARDEN_DATA, this, std::placeholders::_1));
}

CWorldSocket::~CWorldSocket()
{
}

void CWorldSocket::Open(std::string Host, uint16 Port)
{
	bool result = false;

	if (::isdigit(Host.at(0)))
	{
		IPAddress addr(Host);

		result = Connect(addr, Port);
	}
	else
	{
		for (auto addr : Dns::Resolve(Host))
		{
			result = Connect(addr, Port);
			if (result)
				break;
		}
	}

	if (false == result)
	{
		Log::Error("CWorldSocket: Unable to connect to '%s:%d'", Host.c_str(), Port);
		return;
	}

	SetBlocking(false);
}

void CWorldSocket::Update()
{
	CByteBuffer buffer;
	Receive(buffer, 4096 * 16);

	if (buffer.getSize() == 0)
	{
		if (GetStatus() != CSocket::Connected)
		{
			Log::Error("DISCONNECTED.");
		}
		return;
	}

	// Append to current packet
	if (m_CurrentPacket != nullptr)
	{
		Packet2(buffer);
	}

	while (false == buffer.isEof())
	{
		uint8* data = buffer.getDataFromCurrentEx();
		uint8  sizeBytes = sizeof(uint16);
		uint32 size = 0;
		uint16 cmd = 0;

#if 1
		// 1. Decrypt size
		m_WoWCryptoUtils.DecryptRecv(data + 0, 1);
		uint8 firstByte = data[0];

		// 2. Decrypt other size
		if ((firstByte & 0x80) != 0)
		{
			sizeBytes = 3;
			m_WoWCryptoUtils.DecryptRecv(data + 1, 1 + sizeBytes);
			size = (((data[0] & 0x7F) << 16) | (data[1] << 8) | data[2]);
			cmd = ((data[4] << 8) | data[3]);
		}
		else
		{
			sizeBytes = 2;
			m_WoWCryptoUtils.DecryptRecv(data + 1, 1 + sizeBytes);
			size = ((data[0] << 8) | data[1]);
			cmd = ((data[3] << 8) | data[2]);
		}
#else
		// Decrypt size and header
		m_WoWCryptoUtils.DecryptRecv(data + 0, sizeof(uint16) + sizeof(uint16));

		// Check compressed packets
		_ASSERT((data[0] & 0x80) == 0);

		// Size and opcode
		size = ((data[0] << 8) | data[1]);
		cmd = ((data[3] << 8) | data[2]);
#endif

		// DEBUG
		//_ASSERT(cmd < Opcodes::NUM_MSG_TYPES);
		//Log::Green("CWorldSocket: Command '%s' (0x%X) size=%d", OpcodesNames[cmd], cmd, size);

		// Seek to data
		buffer.seekRelative(sizeBytes /*Size*/ + sizeof(uint16) /*Opcode*/);

		Packet1(size - sizeof(uint16) /*Opcode*/, static_cast<Opcodes>(cmd));
		Packet2(buffer);
	}
}



//
// CWorldSocket
//
void CWorldSocket::SendPacket(CClientPacket& Packet)
{
    Packet.Complete();

    m_WoWCryptoUtils.EncryptSend(Packet.getDataEx(), sizeof(uint16) /*Size*/ + sizeof(uint32) /*Opcode*/);

    Send(Packet.getDataEx(), Packet.getSize());
}

void CWorldSocket::SetExternalHandler(std::function<bool(CServerPacket&)> Handler)
{
	_ASSERT(Handler != nullptr);
	m_ExternalHandler = Handler;
}



//
// Packets contructor
//
void CWorldSocket::Packet1(uint16 Size, Opcodes Opcode)
{
    m_CurrentPacket = std::make_unique<CServerPacket>(Size, Opcode);
}

void CWorldSocket::Packet2(CByteBuffer& _buf)
{
	_ASSERT(m_CurrentPacket != nullptr);

    uint32 needToRead = m_CurrentPacket->GetPacketSize() - m_CurrentPacket->getSize();
    if (needToRead > 0)
    {
        uint32 incomingBufferSize = _buf.getSize() - _buf.getPos();

        if (needToRead > incomingBufferSize)
            needToRead = incomingBufferSize;

		if (needToRead > 0)
		{
			// Fill data
			_ASSERT(_buf.getPos() + needToRead <= _buf.getSize());
			m_CurrentPacket->writeBytes( _buf.getDataFromCurrent(), needToRead);

			_buf.seekRelative(needToRead);
			//Log::Info("Packet[%s] readed '%d' of %d'.", OpcodesNames[m_CurrentPacket->GetPacketOpcode()].c_str(), m_CurrentPacket->getSize(), m_CurrentPacket->GetPacketSize());
		}
    }

    // Check if we read full packet
    if (m_CurrentPacket->IsComplete())
    {
		CServerPacket packet(*m_CurrentPacket);
		if (false == ProcessPacket(packet))
		{
			if (m_CurrentPacket->GetPacketOpcode() >= NUM_MSG_TYPES)
			{
				Log::Error("Opcode: ID '%d' (0x%X) is bigger then maximum opcode ID. Size: '%d'.", m_CurrentPacket->GetPacketOpcode(), m_CurrentPacket->GetPacketOpcode(), m_CurrentPacket->GetPacketSize());
			}
			else
			{
				Log::Info("Opcode: '%s' (0x%X). Size: '%d' not handled.", OpcodesNames[m_CurrentPacket->GetPacketOpcode()], m_CurrentPacket->GetPacketOpcode(), m_CurrentPacket->GetPacketSize());
			}
		}

        m_CurrentPacket.reset();
    }
    else
    {
        //Log::Info("Packet[%s] is incomplete. Size '%d' of %d'.", OpcodesNames[m_CurrentPacket->GetPacketOpcode()].c_str(), m_CurrentPacket->getSize(), m_CurrentPacket->GetPacketSize());
    }
}





void CWorldSocket::AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler)
{
	_ASSERT(Handler != nullptr);
	m_Handlers.insert(std::make_pair(Opcode, Handler));
}

bool CWorldSocket::ProcessPacket(CServerPacket& ServerPacket)
{
	ServerPacket.seek(0);

	const auto& handler = m_Handlers.find(ServerPacket.GetPacketOpcode());
	if (handler != m_Handlers.end())
	{
		_ASSERT(handler->second != nullptr);
		(handler->second).operator()(ServerPacket);

		if (ServerPacket.getPos() != ServerPacket.getSize())
			throw CException("CWorldSocket::ProcessPacket: Packet '%d' is not fully readed. %d of %d.", ServerPacket.GetPacketOpcode(), ServerPacket.getPos(), ServerPacket.getSize());

		return true;
	}
	else if (m_ExternalHandler.operator()(ServerPacket))
	{
		return true;
	}

	return false;
}



//
// Handlers
//
void CWorldSocket::On_SMSG_AUTH_CHALLENGE(CServerPacket& _buff)
{
	// Receive
	uint32 serverRandomSeed; 
	_buff.seekRelative(4);
	_buff.readBytes(&serverRandomSeed, 4);
	_buff.seekRelative(32);

	BigNumber clientRandomSeed;
    clientRandomSeed.SetRand(4 * 8);

	uint8 zeroBytes[] = { 0, 0, 0, 0 };

	SHA1Hash uSHA;
	uSHA.Initialize();
	uSHA.UpdateData((const uint8*)m_Login.c_str(), m_Login.size());
	uSHA.UpdateData(zeroBytes, 4);
    uSHA.UpdateBigNumbers(&clientRandomSeed, nullptr);
	uSHA.UpdateData(&reinterpret_cast<uint8&>(serverRandomSeed), 4);
	uSHA.UpdateBigNumbers(&m_Key, nullptr);
	uSHA.Finalize();

	// Send auth packet to server. 

	/*
	    uint32 BattlegroupID = 0;
    uint32 LoginServerType = 0;
    uint32 RealmID = 0;
    uint32 Build = 0;
    uint32 LocalChallenge = 0;
    uint32 LoginServerID = 0;
    uint32 RegionID = 0;
    uint64 DosResponse = 0;
    uint8 Digest[SHA_DIGEST_LENGTH] = {};
    std::string Account;
    ByteBuffer AddonInfo;


		// Read the content of the packet
	recvPacket >> authSession->Build;
	recvPacket >> authSession->LoginServerID;
	recvPacket >> authSession->Account;
	recvPacket >> authSession->LoginServerType;
	recvPacket >> authSession->LocalChallenge;
	recvPacket >> authSession->RegionID;
	recvPacket >> authSession->BattlegroupID;
	recvPacket >> authSession->RealmID;               // realmId from auth_database.realmlist table
	recvPacket >> authSession->DosResponse;
	recvPacket.read(authSession->Digest, 20);
	authSession->AddonInfo.append(recvPacket.contents() + recvPacket.rpos(), recvPacket.size() - recvPacket.rpos());

	*/
	

    CClientPacket p(CMSG_AUTH_SESSION);
    p << (uint32)12340;
    p.writeDummy(4);      // LoginServerID
    p << m_Login;         // Account
	p.writeDummy(4);      // LoginServerType
    p.writeBytes(clientRandomSeed.AsByteArray(4).get(), 4);  // LocalChallenge
	p.writeDummy(4); // RegionID
	p.writeDummy(4); // BattlegroupID
	p << 1;          // RealmID !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	p.writeDummy(8); // DosResponse
    p.writeBytes(uSHA.GetDigest(), SHA_DIGEST_LENGTH); // Digest

    // We must pass addons to connect to private servers
    CByteBuffer addonsBuffer;
	CreateAddonsBuffer(addonsBuffer);

    p.writeBytes(addonsBuffer.getData(), addonsBuffer.getSize());

    SendPacket(p);

	// Start encription from here
    m_WoWCryptoUtils.Init(&m_Key);
}

void CWorldSocket::On_SMSG_AUTH_RESPONSE(CServerPacket& _buff)
{
	enum CommandDetail : uint8
	{
        AUTH_OK = 0x0C,
        AUTH_FAILED = 0x0D,
        AUTH_REJECT = 0x0E,
        AUTH_BAD_SERVER_PROOF = 0x0F,
        AUTH_UNAVAILABLE = 0x10,
        AUTH_SYSTEM_ERROR = 0x11,
        AUTH_BILLING_ERROR = 0x12,
        AUTH_BILLING_EXPIRED = 0x13,
        AUTH_VERSION_MISMATCH = 0x14,
        AUTH_UNKNOWN_ACCOUNT = 0x15,
        AUTH_INCORRECT_PASSWORD = 0x16,
        AUTH_SESSION_EXPIRED = 0x17,
        AUTH_SERVER_SHUTTING_DOWN = 0x18,
        AUTH_ALREADY_LOGGING_IN = 0x19,
        AUTH_LOGIN_SERVER_NOT_FOUND = 0x1A,
        AUTH_WAIT_QUEUE = 0x1B,
        AUTH_BANNED = 0x1C,
        AUTH_ALREADY_ONLINE = 0x1D,
        AUTH_NO_TIME = 0x1E,
        AUTH_DB_BUSY = 0x1F,
        AUTH_SUSPENDED = 0x20,
        AUTH_PARENTAL_CONTROL = 0x21,
        AUTH_LOCKED_ENFORCED = 0x02, /// Unsure
	};

	CommandDetail detail;
	_buff.readBytes(&detail, sizeof(uint8));
	_buff.seekRelative(_buff.getSize() - _buff.getPos());

	if (detail == CommandDetail::AUTH_OK)
	{
        CClientPacket p(CMSG_CHAR_ENUM);
		SendPacket(p);

		Log::Green("Succes access to server!!!");
	}
	else if (detail == CommandDetail::AUTH_WAIT_QUEUE)
	{
		Log::Warn("You in queue!");
	}
	else if (detail == CommandDetail::AUTH_FAILED)
	{
		Log::Warn("Auth failed");
	}
}

void CWorldSocket::On_SMSG_WARDEN_DATA(CServerPacket& Buffer)
{
	if (m_Warden)
		m_Warden->ProcessPacket(Buffer);
}


void CWorldSocket::CreateAddonsBuffer(CByteBuffer& AddonsBuffer)
{
    // This is deafult for blizzard addons
    uint32 blizzardCrc  = 0x1c776d01LL;
    
    std::vector<std::string> blizzardAddOns;
    blizzardAddOns.push_back("Blizzard_AuctionUI");
    blizzardAddOns.push_back("Blizzard_BattlefieldMinimap");
    blizzardAddOns.push_back("Blizzard_BindingUI");
    blizzardAddOns.push_back("Blizzard_CombatText");
    blizzardAddOns.push_back("Blizzard_CraftUI");
    blizzardAddOns.push_back("Blizzard_GMSurveyUI");
    blizzardAddOns.push_back("Blizzard_InspectUI");
    blizzardAddOns.push_back("Blizzard_MacroUI");
    blizzardAddOns.push_back("Blizzard_RaidUI");
    blizzardAddOns.push_back("Blizzard_TalentUI");
    blizzardAddOns.push_back("Blizzard_TradeSkillUI");
    blizzardAddOns.push_back("Blizzard_TrainerUI");

    CByteBuffer addonsBuffer;
    for (auto addonName : blizzardAddOns)
    {
        addonsBuffer << addonName;
        addonsBuffer << (uint32) blizzardCrc;
        addonsBuffer << (uint32) 0x00;
        addonsBuffer << (uint8)  0x00;
    }

    AddonsBuffer << static_cast<uint32>(addonsBuffer.getSize());
    AddonsBuffer.writeDummy(addonsBuffer.getSize());
	AddonsBuffer.seek(0);

    uLongf destLen = addonsBuffer.getSize();
    if (compress(AddonsBuffer.getDataFromCurrentEx() + sizeof(uint32), &destLen, addonsBuffer.getData(), addonsBuffer.getSize()) != Z_OK)
		throw CException("CWorldSocket::CreateAddonsBuffer: Unable to compress addons buffer.");

    //                  addonsRealSize + compressedSize
    //AddonsBuffer.Resize(sizeof(uint32) + destLen);
}

#endif
