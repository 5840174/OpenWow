#pragma once

#include "AuthCodes.h"

#include "SHA1.h"
#include "BigNumber.h"

// FORWARD BEGIN
class CWoWClient;
// FORWARD END

class CAuthSocket : public sockets::TcpSocket
{
	typedef bool (CAuthSocket::* HandlerFunc)(CByteBuffer&);
public:
	CAuthSocket(sockets::ISocketHandler& SocketHandler, CWoWClient& WoWClient, const std::string& Login, const std::string& Password);
	virtual ~CAuthSocket();

	void SendData(const IByteBuffer& _bb);
	void SendData(const uint8* _data, uint32 _count);

	// Handlers
	void ProcessHandler(eAuthCmd AuthCmd, CByteBuffer& _buffer);

	void C_SendLogonChallenge();

	bool S_LoginChallenge(CByteBuffer& _buff);
	bool S_LoginProof(CByteBuffer& _buff);
	bool S_Realmlist(CByteBuffer& _buff);

protected:
	// TcpSocket
	virtual void OnConnect() override final;
	virtual void OnDisconnect() override final;
	virtual void OnRawData(const char *buf, size_t len) override final;

private:
	std::unordered_map<eAuthCmd, HandlerFunc> m_Handlers;

	BigNumber Key;
	SHA1Hash  MServer;

private:
	CWoWClient& m_WoWClient;
	std::string m_Login;
	SHA1Hash    m_LoginPasswordHash;
};