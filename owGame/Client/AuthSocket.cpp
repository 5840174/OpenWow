#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// Include
#include "Client.h"

// General
#include "AuthSocket.h"

CAuthSocket::CAuthSocket(sockets::ISocketHandler& SocketHandler, CWoWClient& WoWClient, const std::string& Login, const std::string& Password)
    : sockets::TcpSocket(SocketHandler)
    , m_WoWClient(WoWClient)
	, m_Login(Login)
{
	char loginPasswordUpperCase[256];
	std::memset(loginPasswordUpperCase, 0x00, sizeof(loginPasswordUpperCase));
	sprintf_s(loginPasswordUpperCase, "%s:%s", m_Login.c_str(), Password.c_str());

	m_LoginPasswordHash.Initialize();
	m_LoginPasswordHash.UpdateData((const uint8*)loginPasswordUpperCase, strlen(loginPasswordUpperCase));
	m_LoginPasswordHash.Finalize();

	m_Handlers[AUTH_LOGON_CHALLENGE] = &CAuthSocket::S_LoginChallenge;
	m_Handlers[AUTH_LOGON_PROOF] = &CAuthSocket::S_LoginProof;
	m_Handlers[REALM_LIST] = &CAuthSocket::S_Realmlist;
}

CAuthSocket::~CAuthSocket()
{
    Log::Info("[AuthSocket]: Deleted.");
}

//--

void CAuthSocket::SendData(const IByteBuffer& _bb)
{
    SendBuf(reinterpret_cast<const char*>(_bb.getData()), _bb.getSize());
}
void CAuthSocket::SendData(const uint8* _data, uint32 _count)
{
    SendBuf(reinterpret_cast<const char*>(_data), _count);
}







//
// Handlers
//
void CAuthSocket::ProcessHandler(eAuthCmd AuthCmd, CByteBuffer& _buffer)
{
    const std::unordered_map<eAuthCmd, HandlerFunc>::iterator& handler = m_Handlers.find(AuthCmd);
    if (handler != m_Handlers.end())
    {
        ((*this).*(handler->second))(_buffer);
    }
    else
    {
        _ASSERT(false, "Handler [0x%X] not found!", AuthCmd);
    }
}

//-- Client to server

namespace
{
	struct AuthChallenge_C
	{
		AuthChallenge_C(std::string Login, uint32 IPv4)
			: Login(Login)
			, IPv4(IPv4)
		{}

		void Send(sockets::TcpSocket* _socket)
		{
			CByteBuffer bb;
			bb << (uint8)AUTH_LOGON_CHALLENGE;
			bb << (uint8)6;
			bb << (uint8)(Login.size() + 30);
			bb << (uint8)0;

			bb.writeBytes(gamename, 4);
			bb << version1;
			bb << version2;
			bb << version3;
			bb << build;
			bb.writeBytes((const uint8*)"68x", 4);   // x86
			bb.writeBytes((const uint8*)"niW", 4);   // Win
			bb.writeBytes((const uint8*)"URur", 4);  // enUS
			bb << (uint32)180;
			bb << IPv4;
			bb << (uint8)Login.size();
			bb.writeString(Login);

			_socket->SendBuf(reinterpret_cast<const char*>(bb.getData()), bb.getSize());
		}

		//--

		uint8   gamename[4] = {};
		const uint8 version1 = 3;
		const uint8 version2 = 3;
		const uint8 version3 = 5;
		const uint16 build = 12340;
		uint32 IPv4;
		std::string	Login;
	};
}

void CAuthSocket::C_SendLogonChallenge()
{
    AuthChallenge_C challenge(m_Login, sockets::TcpSocket::GetSockIP4());
    challenge.Send(this);
}

//-- Server to client

namespace
{
	struct AuthProof_C
	{
		AuthProof_C(uint8* _A, const uint8* _MClient)
		{
			std::memcpy(A, _A, 32);
			std::memcpy(M1, _MClient, SHA_DIGEST_LENGTH);
		}

		void Send(sockets::TcpSocket* _socket)
		{
			CByteBuffer bb;
			bb << (uint8)AUTH_LOGON_PROOF;
			bb.writeBytes(A, 32);
			bb.writeBytes(M1, SHA_DIGEST_LENGTH);
			bb.writeDummy(20);
			bb << (uint8)0;
			bb << (uint8)0;

			_socket->SendBuf(reinterpret_cast<const char*>(bb.getData()), bb.getSize());
		}

		uint8 A[32];
		uint8 M1[SHA_DIGEST_LENGTH];
	};
}

bool CAuthSocket::S_LoginChallenge(CByteBuffer& _buff)
{
    _buff.seekRelative(sizeof(uint8));

    eAuthResults error;
    _buff.readBytes(&error, 1);

    if (error != eAuthResults::REALM_AUTH_SUCCESS)
    {
        Log::Error("Server challege not accept (0x%X)", error);
        return false;
    }

#pragma region Receive and initialize
    // server public key
    uint8 B_raw[32];
    _buff.readBytes(B_raw, 32);
    BigNumber B;
    B.SetBinary(B_raw, 32);					

    // 'g'
    uint8 g_rawLen;
    _buff.readBytes(&g_rawLen, sizeof(uint8));
    uint8 g_raw[1];
    _buff.readBytes(g_raw, g_rawLen);
    BigNumber g;
    g.SetBinary(g_raw, g_rawLen);

    // modulus
    uint8 N_rawLen;
    _buff.readBytes(&N_rawLen, sizeof(uint8));
    uint8 N_raw[32];
    _buff.readBytes(N_raw, N_rawLen);
    BigNumber N;
    N.SetBinary(N_raw, N_rawLen);

    // s
    uint8 s_raw[32];
    _buff.readBytes(s_raw, 32);
    BigNumber s;
    s.SetBinary(s_raw, 32);

    _buff.seekRelative(sizeof(uint8) * 16);
    _buff.seekRelative(sizeof(uint8));
    
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

    AuthProof_C authProof(A.AsByteArray(32).get(), MClient.GetDigest());
    authProof.Send(this);

    // Expected proof for server
    MServer.Initialize();
    MServer.UpdateBigNumbers(&A, nullptr);
    MServer.UpdateData(MClient.GetDigest(), SHA_DIGEST_LENGTH);
    MServer.UpdateBigNumbers(&Key, nullptr);
    MServer.Finalize();

    return true;
}

bool CAuthSocket::S_LoginProof(CByteBuffer& _buff)
{
    uint8 error;
    _buff.read(&error);

    if (error != eAuthResults::REALM_AUTH_SUCCESS)
    {
        switch (error)
        {
            case eAuthResults::REALM_AUTH_WRONG_BUILD_NUMBER:
                Log::Error("REALM_AUTH_WRONG_BUILD_NUMBER");
            break;

            case eAuthResults::REALM_AUTH_NO_MATCH:
                Log::Error("REALM_AUTH_NO_MATCH");
            break;
        }

        return false;
    }

    uint8 M2[20];
    _buff.readBytes(M2, 20);

    uint32 AccountFlags;
    _buff.read(&AccountFlags);

    // Server M must be same with client M
    if (MServer != M2)
    {
        Log::Error("Server 'M' mismatch!");
        return false;
    }

    Log::Green("All ok! Server proof equal client calculated server proof!");
    Log::Green("Successfully logined!!!");

    // TODO: refactor realms

    CByteBuffer bb2;
    bb2 << (uint8)REALM_LIST;
    bb2 << (uint32)0;
    SendData(bb2);

    return true;
}

bool CAuthSocket::S_Realmlist(CByteBuffer& _buff)
{
    uint16 bufferSize;
    _buff.read(&bufferSize);

    uint32 realmlistBufferSize;
    _buff.read(&realmlistBufferSize);

    uint8 count;
    _buff.read(&count);
    Log::Green("S_Realmlist: Count [%d]", count);

    for (uint32 i = 0; i < count; i++)
    {
        RealmInfo rinfo(_buff);
		m_WoWClient.AddRealm(rinfo);
    }

	m_WoWClient.OnSuccessConnect(Key);

    return true;
}



//
// TcpSocket
//
void CAuthSocket::OnConnect()
{
	C_SendLogonChallenge();
}

void CAuthSocket::OnDisconnect()
{

}

void CAuthSocket::OnRawData(const char * buf, size_t len)
{
	CByteBuffer bb(buf, len);

	eAuthCmd currHandler;
	bb.readBytes(&currHandler, sizeof(uint8));

	ProcessHandler(currHandler, bb);
}

#endif