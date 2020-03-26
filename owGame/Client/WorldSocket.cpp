#include "stdafx.h"

// Include
#include "Client.h"

// General
#include "WorldSocket.h"

// Additional
#include <zlib\\zlib.h>
#include "OpcodesNames.h"
#include "HMACSHA1.h"

const Opcodes IgnoredOpcodes[] = 
{
	Opcodes::SMSG_SET_PROFICIENCY,
	Opcodes::SMSG_LOGIN_VERIFY_WORLD,
	Opcodes::SMSG_ACCOUNT_DATA_TIMES,
	Opcodes::SMSG_FEATURE_SYSTEM_STATUS,
	//Opcodes::SMSG_BINDPOINTUPDATE,
	Opcodes::SMSG_INITIAL_SPELLS,
	Opcodes::SMSG_SEND_UNLEARN_SPELLS,
	Opcodes::SMSG_ACTION_BUTTONS,
	Opcodes::SMSG_INITIALIZE_FACTIONS,
	//Opcodes::SMSG_LOGIN_SETTIMESPEED,
	Opcodes::SMSG_SET_FORCED_REACTIONS,
	Opcodes::SMSG_COMPRESSED_UPDATE_OBJECT,
	Opcodes::SMSG_MONSTER_MOVE,

	Opcodes::SMSG_EMOTE,
	Opcodes::SMSG_TIME_SYNC_REQ,
	Opcodes::SMSG_SPELL_START,
	Opcodes::SMSG_SPELL_GO
};

CWorldSocket::CWorldSocket(sockets::ISocketHandler& SocketHandler, const std::string& Login, BigNumber Key)
	: TcpSocket(SocketHandler)
	, m_CurrentPacket(nullptr)
	, m_Login(Login)
	, m_Key(Key)
{}

CWorldSocket::~CWorldSocket()
{
	Log::Info("[WorldSocket]: All threads stopped.");
}



//
// TcpSocket
//
void CWorldSocket::OnConnect()
{
}

void CWorldSocket::OnDisconnect()
{
    SetErasedByHandler();
}

void CWorldSocket::OnRawData(const char * buf, size_t len)
{
    //Log::Info("CWorldSocket: Receive data with size=%d", len);

    CByteBuffer bufferFromServer(buf, len);
	
    // Append to current packet
    if (m_CurrentPacket != nullptr)
    {
        Packet2(bufferFromServer);
    }

    while (!bufferFromServer.isEof())
    {
        uint8* data = bufferFromServer.getDataFromCurrentEx();
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

        //
        bool needMess = true;
        for (uint32 i = 0; i < OW_COUNT_ELEMENTS(IgnoredOpcodes); i++)
        {
            if (IgnoredOpcodes[i] == cmd)
            {
                needMess = false;
                break;
            }
        }

        // DEBUG
        _ASSERT(cmd < Opcodes::NUM_MSG_TYPES);
        //Log::Green("CWorldSocket: Command '%s' (0x%X) size=%d", OpcodesNames[cmd], cmd, size);

        // Seek to data
        bufferFromServer.seekRelative(sizeBytes /*Size*/ + sizeof(uint16) /*Opcode*/);

        Packet1(size - sizeof(uint16) /*Opcode*/, static_cast<Opcodes>(cmd));
        Packet2(bufferFromServer);
    }
}



//
// CWorldSocket
//
void CWorldSocket::SendPacket(CClientPacket& Packet)
{
    Packet.Complete();

    m_WoWCryptoUtils.EncryptSend(Packet.getDataEx(), sizeof(uint16) /*Size*/ + sizeof(uint32) /*Opcode*/);

    SendBuf(reinterpret_cast<const char*>(Packet.getDataEx()), Packet.getSize());
}

void CWorldSocket::SetExternalHandler(std::function<bool(Opcodes, CServerPacket&)> Handler)
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
        {
            needToRead = incomingBufferSize;
        }

		if (needToRead > 0)
		{
			// Fill data
			_ASSERT(_buf.getPos() + needToRead <= _buf.getSize());
			m_CurrentPacket->writeBytes(_buf.getDataFromCurrent(), needToRead);

			_buf.seekRelative(needToRead);
			//Log::Info("Packet[%s] readed '%d' of %d'.", OpcodesNames[m_CurrentPacket->GetPacketOpcode()].c_str(), m_CurrentPacket->getSize(), m_CurrentPacket->GetPacketSize());
		}
    }

    // Check if we read full packet
    if (m_CurrentPacket->IsComplete())
    {
        if (! ProcessPacket(*m_CurrentPacket))
			Log::Green("Opcode: '%s' (0x%X). Size: '%d'", OpcodesNames[m_CurrentPacket->GetPacketOpcode()], m_CurrentPacket->GetPacketOpcode(), m_CurrentPacket->GetPacketSize());

        m_CurrentPacket.reset();
    }
    else
    {
        //Log::Info("Packet[%s] is incomplete. Size '%d' of %d'.", OpcodesNames[m_CurrentPacket->GetPacketOpcode()].c_str(), m_CurrentPacket->getSize(), m_CurrentPacket->GetPacketSize());
    }
}

/*void CWorldSocket::InitHandlers()
{
	m_Handlers[SMSG_AUTH_CHALLENGE] = std::bind(&CWorldSocket::S_AuthChallenge, this, std::placeholders::_1);
	m_Handlers[SMSG_AUTH_RESPONSE] = std::bind(&CWorldSocket::S_AuthResponse, this, std::placeholders::_1);

	// Dummy
	m_Handlers[SMSG_SET_PROFICIENCY] = nullptr;
	m_Handlers[SMSG_ACCOUNT_DATA_TIMES] = nullptr;
	m_Handlers[SMSG_FEATURE_SYSTEM_STATUS] = nullptr;
	m_Handlers[SMSG_BINDPOINTUPDATE] = nullptr;
	m_Handlers[SMSG_INITIAL_SPELLS] = nullptr;
	m_Handlers[SMSG_SEND_UNLEARN_SPELLS] = nullptr;
	m_Handlers[SMSG_ACTION_BUTTONS] = nullptr;
	m_Handlers[SMSG_INITIALIZE_FACTIONS] = nullptr;
	m_Handlers[SMSG_LOGIN_SETTIMESPEED] = nullptr;
	m_Handlers[SMSG_SET_FORCED_REACTIONS] = nullptr;

	m_Handlers[Opcodes::SMSG_EMOTE] = nullptr;
	m_Handlers[Opcodes::SMSG_TIME_SYNC_REQ] = nullptr;
	m_Handlers[Opcodes::SMSG_SPELL_START] = nullptr;
	m_Handlers[Opcodes::SMSG_SPELL_GO] = nullptr;
}*/

bool CWorldSocket::ProcessPacket(CServerPacket ServerPacket)
{
	if (ServerPacket.GetPacketOpcode() == SMSG_AUTH_CHALLENGE)
	{
		S_AuthChallenge(ServerPacket);
		return true;
	}
	else if (ServerPacket.GetPacketOpcode() == SMSG_AUTH_RESPONSE)
	{
		S_AuthResponse(ServerPacket);
		return true;
	}
	else if (m_ExternalHandler.operator()(ServerPacket.GetPacketOpcode(), ServerPacket))
	{
		return true;
	}

	return false;
}



//
// Handlers
//
void CWorldSocket::S_AuthChallenge(CByteBuffer& _buff)
{
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
    CClientPacket p(CMSG_AUTH_SESSION);
    p << (uint32)12340;
    p.writeDummy(4);
    p << m_Login;
	p.writeDummy(4);
    p.writeBytes(clientRandomSeed.AsByteArray(4).get(), 4);
	p.writeDummy(4);
	p.writeDummy(4);
	p << 1;
	p.writeDummy(8);
    p.writeBytes(uSHA.GetDigest(), SHA_DIGEST_LENGTH);

    // We must pass addons to connect to private servers
    CByteBuffer addonsBuffer;
	CreateAddonsBuffer(addonsBuffer);
    p.writeBytes(addonsBuffer.getData(), addonsBuffer.getSize());

    SendPacket(p);

	// Start encription from here
    m_WoWCryptoUtils.Init(&m_Key);
}

void CWorldSocket::S_AuthResponse(CByteBuffer& _buff)
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
    {
        Log::Error("Can't compress addons.");
        _ASSERT(false);
    }

    //                  addonsRealSize + compressedSize
    //AddonsBuffer.Resize(sizeof(uint32) + destLen);
}
