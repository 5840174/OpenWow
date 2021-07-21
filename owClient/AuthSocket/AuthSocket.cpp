#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// Include
#include "../Client/Client.h"

// General
#include "AuthSocket.h"

namespace
{
	struct SAuthChallenge
		: public CByteBuffer
	{
		SAuthChallenge(std::string Login, uint32 IPv4)
		{
			(*this) << (uint8)AUTH_LOGON_CHALLENGE;
			(*this) << (uint8)6;
			(*this) << (uint8)(Login.size() + 30);
			(*this) << (uint8)0;

			(*this).writeBytes(gamename, 4);
			(*this).writeBytes(version, 3);
			(*this) << build;
			(*this).writeBytes((const uint8*)"68x", 4);   // x86
			(*this).writeBytes((const uint8*)"niW", 4);   // Win
			(*this).writeBytes((const uint8*)"URur", 4);  // enUS
			(*this) << (uint32)180;
			(*this) << IPv4;
			(*this) << (uint8)Login.size();
			(*this).writeBytes(Login.c_str(), Login.length());
		}

		const uint8 gamename[4] = {};
		const uint8 version[3] = { 3, 3, 5 };
		const uint16 build = 12340;
	};

	struct SAuthProof
		: public CByteBuffer
	{
		SAuthProof(uint8* A, const uint8* MClient)
		{
			(*this) << (uint8)AUTH_LOGON_PROOF;
			(*this).writeBytes(A, 32);
			(*this).writeBytes(MClient, SHA_DIGEST_LENGTH);
			(*this).writeDummy(20);
			(*this) << (uint8)0;
			(*this) << (uint8)0;
		}
	};
}

CAuthSocket::CAuthSocket(CowClient& WoWClient, const std::string& Login, const std::string& Password)
    : m_WoWClient(WoWClient)
	, m_Login(Login)
{
	std::string loginPasswordUpperCase = Login + ":" + Password;

	m_LoginPasswordHash.Initialize();
	m_LoginPasswordHash.UpdateData((const uint8*)loginPasswordUpperCase.c_str(), loginPasswordUpperCase.length());
	m_LoginPasswordHash.Finalize();

	AddHandler(AUTH_LOGON_CHALLENGE, std::bind(&CAuthSocket::On_AUTH_LOGON_CHALLENGE, this, std::placeholders::_1));
	AddHandler(AUTH_LOGON_PROOF, std::bind(&CAuthSocket::On_AUTH_LOGON_PROOF, this, std::placeholders::_1));
	AddHandler(REALM_LIST, std::bind(&CAuthSocket::On_REALM_LIST, this, std::placeholders::_1));
}

CAuthSocket::~CAuthSocket()
{
    Log::Info("CAuthSocket: Deleted.");
}

void CAuthSocket::Open(std::string Host, uint16 Port)
{
	if (false == Connect(Host, Port))
	{
		Log::Error("CAuthSocket: Unable to connect to '%s:%d'", Host.c_str(), Port);
		return;
	}

	SetBlocking(false);
}

void CAuthSocket::Update()
{
	CByteBuffer buffer;
	Receive(buffer, 4096);

	if (buffer.getSize() == 0)
	{
		if (GetStatus() != CSocket::Connected)
		{
			//NotifyListeners(&ConnectionListener::OnSocketStateChange, m_TCPSocket.GetStatus());
		}
		return;
	}

	EAuthCmd authCmd;
	buffer >> authCmd;
	ProcessHandler(authCmd, buffer);
}

//--

void CAuthSocket::OnConnected()
{
	Log::Green("CAuthSocket::OnConnected.");
	SendData(SAuthChallenge(m_Login, GetMyIP()));
}

void CAuthSocket::OnDisconnected()
{
	Log::Warn("CAuthSocket::OnDisconnected.");
}

void CAuthSocket::SendData(const IByteBuffer& Buffer)
{
	Send(Buffer.getData(), Buffer.getSize());
}




//
// Private
//
void CAuthSocket::AddHandler(EAuthCmd AuthCmd, std::function<bool(CByteBuffer&)> Handler)
{
	_ASSERT(Handler != nullptr);
	m_Handlers.insert(std::make_pair(AuthCmd, Handler));
}

void CAuthSocket::ProcessHandler(EAuthCmd AuthCmd, CByteBuffer& _buffer)
{
    const auto& handler = m_Handlers.find(AuthCmd);
	if (handler != m_Handlers.end())
	{
		handler->second.operator()(_buffer);
	}
	else
		throw CException("CAuthSocket::ProcessHandler: Handler for AuthCmd '0x%X' not found.", AuthCmd);
}

bool CAuthSocket::On_AUTH_LOGON_CHALLENGE(CByteBuffer& Buffer)
{
	uint8 zero;
    Buffer >> zero;
	if (zero != 0x00)
		throw CException("CAuthSocket::On_AUTH_LOGON_CHALLENGE: Zeros mismatch.");

    EAuthResults error;
    Buffer >> error;
    if (error != EAuthResults::WOW_SUCCESS)
    {
        Log::Error("CAuthSocket::On_AUTH_LOGON_CHALLENGE: Logon challenge don't accept. Error: '0x%20X'.", error);
        return false;
    }

#pragma region Receive and initialize
    // server public key
    uint8 B_raw[32];
    Buffer.readBytes(B_raw, 32);
    BigNumber B;
    B.SetBinary(B_raw, 32);					

    // 'g'
    uint8 g_rawLen;
    Buffer.readBytes(&g_rawLen, sizeof(uint8));
    uint8 g_raw[1];
    Buffer.readBytes(g_raw, g_rawLen);
    BigNumber g;
    g.SetBinary(g_raw, g_rawLen);

    // modulus
    uint8 N_rawLen;
    Buffer.readBytes(&N_rawLen, sizeof(uint8));
    uint8 N_raw[32];
    Buffer.readBytes(N_raw, N_rawLen);
    BigNumber N;
    N.SetBinary(N_raw, N_rawLen);

    // s
    uint8 s_raw[32];
    Buffer.readBytes(s_raw, 32);
    BigNumber s;
    s.SetBinary(s_raw, 32);

    Buffer.seekRelative(sizeof(uint8) * 16);
    Buffer.seekRelative(sizeof(uint8));
    
    //Log::Info("---====== Received from server: ======---");
    //Log::Info("B=%s", B.AsHexStr().c_str());
    //Log::Info("g=%s", B.AsHexStr().c_str());
    //Log::Info("N=%s", N.AsHexStr().c_str());
    //Log::Info("s=%s", s.AsHexStr().c_str());
#pragma endregion

#pragma region Hash password
    // x = H(s, p) (пользователь вводит пароль) salt + password
    SHA1Hash xSHA;
    xSHA.Initialize();
    xSHA.UpdateData(s.AsByteArray().get(), 32);
    xSHA.UpdateData(m_LoginPasswordHash.GetDigest(), SHA_DIGEST_LENGTH);
    xSHA.Finalize();

    BigNumber x;
    x.SetBinary(xSHA.GetDigest(), SHA_DIGEST_LENGTH);

    //Log::Info("---====== shared password hash ======---");
    //Log::Info("p=%s", PasswordHash.toString().c_str());
    //Log::Info("x=%s", x.AsHexStr().c_str());
#pragma endregion

#pragma region Create random key pair
    BigNumber a;
    a.SetRand(19 * 8);

    BigNumber A = g.ModExp(a, N);

    //Log::Info("---====== Send data to server: ======---");
    //Log::Info("a=%s", a.AsHexStr().c_str());
    //Log::Info("A=%s", A.AsHexStr().c_str());
#pragma endregion

#pragma region Compute session key
    SHA1Hash uSHA;
    uSHA.Initialize();
    uSHA.UpdateBigNumbers(&A, &B, nullptr);
    uSHA.Finalize();

    BigNumber u;
    u.SetBinary(uSHA.GetDigest(), SHA_DIGEST_LENGTH);

    // compute session key
    // TODO: session key computation fails for some reason all variables match exactly to the server side, but S is different
    BigNumber S = (B - BigNumber(3) * g.ModExp(x, N)).ModExp(a + u * x, N);

    uint8 keyData[40];

    uint8 t[32];
    memcpy(t, S.AsByteArray(32).get(), 32);

    // take every even indices byte, hash, store in even indices
    uint8 t1[16];
    for (int i = 0; i < 16; ++i) t1[i] = t[i * 2];

    SHA1Hash sha;
    sha.Initialize();
    sha.UpdateData(t1, 16);
    sha.Finalize();
    for (uint32 i = 0; i < SHA_DIGEST_LENGTH; ++i) keyData[i * 2] = sha.GetDigest()[i];

    // do the same for odd indices
    for (uint32 i = 0; i < 16; ++i) t1[i] = t[i * 2 + 1];

    sha.Initialize();
    sha.UpdateData(t1, 16);
    sha.Finalize();
    for (uint32 i = 0; i < SHA_DIGEST_LENGTH; ++i)	keyData[i * 2 + 1] = sha.GetDigest()[i];

    Key.SetBinary(keyData, SHA_DIGEST_LENGTH * 2);

    //Log::Info("---====== Compute session key ======---");
    //Log::Info("K=%s", Key.AsHexStr().c_str());
#pragma endregion

#pragma region Generate crypto proof
    // H(N)
    sha.Initialize();
    sha.UpdateBigNumbers(&N, nullptr);
    sha.Finalize();
    uint8 HN[SHA_DIGEST_LENGTH];
    memcpy(HN, sha.GetDigest(), SHA_DIGEST_LENGTH);

    // H(g)
    sha.Initialize();
    sha.UpdateBigNumbers(&g, nullptr);
    sha.Finalize();
    uint8 HG[SHA_DIGEST_LENGTH];
    memcpy(HG, sha.GetDigest(), SHA_DIGEST_LENGTH);

    // H(N) xor H(g)
    uint8 HNxorHG[SHA_DIGEST_LENGTH];
    for (uint32 i = 0; i < SHA_DIGEST_LENGTH; ++i)	HNxorHG[i] = HN[i] ^ HG[i];

    // H(I)
    sha.Initialize();
    sha.UpdateData((const uint8*)m_Login.c_str(), m_Login.size());
    sha.Finalize();

    // M
    SHA1Hash MClient;
    MClient.Initialize();
    MClient.UpdateData(HNxorHG, SHA_DIGEST_LENGTH);
    MClient.UpdateData(sha.GetDigest(), SHA_DIGEST_LENGTH);
    MClient.UpdateBigNumbers(&s, &A, &B, &Key, nullptr);
    MClient.Finalize();

    //Log::Info("---====== Send proof to server: ======---");
    //Log::Info("MC=%s", MClient.toString().c_str());
#pragma endregion

	// Expected proof for server
	MServer.Initialize();
	MServer.UpdateBigNumbers(&A, nullptr);
	MServer.UpdateData(MClient.GetDigest(), SHA_DIGEST_LENGTH);
	MServer.UpdateBigNumbers(&Key, nullptr);
	MServer.Finalize();

	// Send proof
	SendData(SAuthProof(A.AsByteArray(32).get(), MClient.GetDigest()));

    return true;
}

bool CAuthSocket::On_AUTH_LOGON_PROOF(CByteBuffer& Buffer)
{
	EAuthResults error;
    Buffer >> error;

    if (error != EAuthResults::WOW_SUCCESS)
    {
		Log::Error("CAuthSocket::On_AUTH_LOGON_PROOF: Logon proof don't accept. Error: '0x%20X'.", error);
		return false;
    }

    uint8 M2[20];
    Buffer.readBytes(M2, 20);

    uint32 accountFlags;
    Buffer.read(&accountFlags);

    // Server M must be same with client M
    if (MServer != M2)
    {
        Log::Error("CAuthSocket::On_AUTH_LOGON_PROOF: Server 'M' mismatch!");
        return false;
    }

    Log::Green("CAuthSocket::On_AUTH_LOGON_PROOF: Successfully logined.");

    // TODO: refactor realms

    CByteBuffer bytes;
	bytes << (uint8)REALM_LIST;
	bytes << (uint32)0;
    SendData(bytes);

    return true;
}

bool CAuthSocket::On_REALM_LIST(CByteBuffer& Buffer)
{
	uint16 packetSize;
	Buffer.read(&packetSize);

	// RealmListSizeBuffer
	Buffer.seekRelative(4); // uint32 (0)

	uint16 realmlistCount;
	Buffer.read(&realmlistCount);

    Log::Green("CAuthSocket::On_REALM_LIST: Count '%d'.", realmlistCount);

	std::vector<SRealmInfo> realmListInfos;
    for (uint32 i = 0; i < realmlistCount; i++)
    {
        SRealmInfo rinfo(Buffer);
		rinfo.ToString();
		realmListInfos.push_back(rinfo);
    }

	m_WoWClient.OnRealmsListObtained(realmListInfos, Key);

    return true;
}

#endif