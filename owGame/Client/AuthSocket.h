#pragma once

#include "AuthCodes.h"

#include "SHA1.h"
#include "BigNumber.h"

// FORWARD BEGIN
class CWoWClient;
// FORWARD END

class CAuthSocket : public TcpSocket
{
	typedef bool (CAuthSocket::* HandlerFunc)(CByteBuffer&);
public:
	CAuthSocket(ISocketHandler& SocketHandler, std::shared_ptr<CWoWClient> WoWClient);
	~CAuthSocket();

	void SendData(const IByteBuffer& _bb);
	void SendData(const uint8* _data, uint32 _count);

    // TcpSocket
    virtual void OnConnect() override final;
    virtual void OnDisconnect() override final;
    virtual void OnRawData(const char *buf, size_t len) override final;

	// Handlers
	void InitHandlers();
	void ProcessHandler(eAuthCmd AuthCmd, CByteBuffer& _buffer);

	void C_SendLogonChallenge();

	bool S_LoginChallenge(CByteBuffer& _buff);
	bool S_LoginProof(CByteBuffer& _buff);
	bool S_Realmlist(CByteBuffer& _buff);

private:
	std::weak_ptr<CWoWClient> m_WoWClient;

	std::unordered_map<eAuthCmd, HandlerFunc> m_Handlers;

	BigNumber Key;
	SHA1Hash  MServer;
};