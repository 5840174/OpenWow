#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "AuthCodes.h"

#include "Cryptography/SHA1.h"
#include "Cryptography/BigNumber.h"

// FORWARD BEGIN
class CWoWClient;
// FORWARD END

class CAuthSocket
	: public CTCPSocket
{
public:
	CAuthSocket(CWoWClient& WoWClient, const std::string& Login, const std::string& Password);
	virtual ~CAuthSocket();

	void Open(std::string Host, uint16 Port);
	void Update();

	void SendData(const IByteBuffer& _bb);
	void SendData(const uint8* _data, uint32 _count);

	// Handlers
	bool On_AUTH_LOGON_CHALLENGE(CByteBuffer& _buff);
	bool On_AUTH_LOGON_PROOF(CByteBuffer& _buff);
	bool On_REALM_LIST(CByteBuffer& _buff);

private:
	void AddHandler(EAuthCmd AuthCmd, std::function<bool(CByteBuffer&)> Handler);
	void ProcessHandler(EAuthCmd AuthCmd, CByteBuffer& _buffer);

private:
	std::unordered_map<EAuthCmd, std::function<void(CByteBuffer&)>> m_Handlers;

	BigNumber Key;
	SHA1Hash  MServer;

private:
	CWoWClient& m_WoWClient;
	std::string m_Login;
	SHA1Hash    m_LoginPasswordHash;
};

#endif