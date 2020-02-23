#pragma once

#include "AuthCrypt.h"
#include "Opcodes.h"
#include "ServerPacket.h"
#include "ClientPacket.h"
#include "RealmInfo.h"

// FORWARD BEGIN
class CWoWClient;
// FORWARD END


struct ServerPktHeader
{
    uint16 size;
    uint16 cmd;
};

struct ClientPktHeader
{
    uint16 size;
    uint32 cmd;
};


class CWorldSocket : public TcpSocket
{
public:
    typedef std::function<void(CByteBuffer&)> HandlerFuncitonType;
public:
	CWorldSocket(ISocketHandler& SocketHandler, CWoWClient& WoWClient);
	virtual ~CWorldSocket();

    // TcpSocket
    void OnConnect() override final;
    void OnDisconnect() override final;
    void OnRawData(const char *buf, size_t len) override final;

	// CWorldSocket
    void SendPacket(CClientPacket& Packet);

    // Packets contructor
    void Packet1(uint16 Size, Opcodes Opcode);
    void Packet2(CByteBuffer& _buf);

	void InitHandlers();
	void AddHandler(Opcodes Opcode, HandlerFuncitonType Handler);
	bool ProcessPacket(CServerPacket ServerPacket);

    // Handlers
	void S_AuthChallenge(CByteBuffer& Buffer);
	void S_AuthResponse(CByteBuffer& Buffer);

private:
    void S_AuthChallenge_CreateAddonsBuffer(CByteBuffer& AddonsBuffer);

private:
	AuthCrypt                                           m_WoWCryptoUtils;

    CServerPacket *                                     m_CurrentPacket;

	std::unordered_map<Opcodes, std::function<void(CByteBuffer&)>>	m_Handlers;

private:
	CWoWClient& m_WoWClient;
};