#include "stdafx.h"

// Include
#include "Client.h"

// General
#include "AuthSocket.h"

CAuthSocket::CAuthSocket(ISocketHandler& SocketHandler, std::shared_ptr<CWoWClient> WoWClient)
    : TcpSocket(SocketHandler)
    , m_WoWClient(WoWClient)
{
    InitHandlers();
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
    CByteBuffer bb;
    bb.Allocate(len);
    bb.CopyData(reinterpret_cast<const uint8*>(buf), len);

    eAuthCmd currHandler;
    bb.readBytes(&currHandler, sizeof(uint8));

    ProcessHandler(currHandler, bb);
}



//
// Handlers
//
void CAuthSocket::InitHandlers()
{
    m_Handlers[AUTH_LOGON_CHALLENGE] = &CAuthSocket::S_LoginChallenge;
    m_Handlers[AUTH_LOGON_PROOF] = &CAuthSocket::S_LoginProof;
    m_Handlers[REALM_LIST] = &CAuthSocket::S_Realmlist;
}

void CAuthSocket::ProcessHandler(eAuthCmd AuthCmd, CByteBuffer& _buffer)
{
    std::unordered_map<eAuthCmd, HandlerFunc>::iterator handler = m_Handlers.find(AuthCmd);
    if (handler != m_Handlers.end())
    {
        (*this.*handler->second)(_buffer);
    }
    else
    {
        Log::Error("Handler [0x%X] not found!", AuthCmd);
    }
}

//-- Client to server
#include "AuthChallenge_C.h"

void CAuthSocket::C_SendLogonChallenge()
{
    std::shared_ptr<CWoWClient> WoWClient = m_WoWClient.lock();
    _ASSERT(WoWClient);

    AuthChallenge_C challenge(WoWClient->GetLogin(), TcpSocket::GetSockIP4());
    challenge.Send(this);
}

//-- Server to client

#include "AuthProof_C.h"

bool CAuthSocket::S_LoginChallenge(CByteBuffer& _buff)
{
    std::shared_ptr<CWoWClient> WoWClient = m_WoWClient.lock();
    _ASSERT(WoWClient);


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
    xSHA.UpdateData(WoWClient->GetLoginPasswordHash().GetDigest(), SHA_DIGEST_LENGTH);
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
    sha.UpdateData((const uint8*)WoWClient->GetLogin().c_str(), WoWClient->GetLogin().size());
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

#pragma region Send proof
    AuthProof_C authProof(A.AsByteArray(32).get(), MClient.GetDigest());
    authProof.Send(this);
#pragma endregion

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
    _buff.readBytes(&error, sizeof(uint8));

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
    _buff.readBytes(&AccountFlags, 4);

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
    std::shared_ptr<CWoWClient> WoWClient = m_WoWClient.lock();
    _ASSERT(WoWClient);

    uint16 bufferSize;
    _buff.readBytes(&bufferSize, 2);

    uint32 realmlistBufferSize;
    _buff.readBytes(&realmlistBufferSize, 4);

    uint8 count;
    _buff.readBytes(&count, sizeof(uint8));
    Log::Green("S_Realmlist: Count [%d]", count);

    for (uint32 i = 0; i < count; i++)
    {
        RealmInfo rinfo(_buff);
        WoWClient->AddRealm(rinfo);
    }

    WoWClient->OnSuccessConnect(Key);

    return true;
}
