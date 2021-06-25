#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "AuthCodes.h"

#include "SHA1.h"
#include "BigNumber.h"

// FORWARD BEGIN
class CWoWClient;
// FORWARD END

class CAuthSocket
	: public CTCPSocket
{
public:
	CAuthSocket(CWoWClient& WoWClient, const std::string& Login, const std::string& Password);
	virtual ~CAuthSocket();

	void Open(std::string Host, port_t Port);
	void Update();

	void SendData(const IByteBuffer& _bb);
	void SendData(const uint8* _data, uint32 _count);

	// Handlers
	void C_SendLogonChallenge();

	bool S_LoginChallenge(CByteBuffer& _buff);
	bool S_LoginProof(CByteBuffer& _buff);
	bool S_Realmlist(CByteBuffer& _buff);

private:
	void AddHandler(eAuthCmd AuthCmd, std::function<bool(CByteBuffer&)> Handler);
	void ProcessHandler(eAuthCmd AuthCmd, CByteBuffer& _buffer);

private:
	std::unordered_map<eAuthCmd, std::function<void(CByteBuffer&)>> m_Handlers;

	BigNumber Key;
	SHA1Hash  MServer;

private:
	CWoWClient& m_WoWClient;
	std::string m_Login;
	SHA1Hash    m_LoginPasswordHash;
};

#endif