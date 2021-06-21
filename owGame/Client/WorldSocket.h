#pragma once

#ifdef ENABLE_WOW_CLIENT

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


class CWorldSocket
{
public:
	CWorldSocket(const std::string& Login, BigNumber Key);
	virtual ~CWorldSocket();

	void Open(std::string Host, port_t Port);
	void Update();

    // TcpSocket
    void OnConnect();
    void OnDisconnect();
    void OnRawData(const char *buf, size_t len);

	// CWorldSocket
    void SendPacket(CClientPacket& Packet);
	void SetExternalHandler(std::function<bool(Opcodes, CServerPacket&)> Handler);
	
private: // Packets contructor
	void Packet1(uint16 Size, Opcodes Opcode);
	void Packet2(CByteBuffer& _buf);
	bool ProcessPacket(CServerPacket ServerPacket);

private: // Used while connect to world
	void S_AuthChallenge(CByteBuffer& Buffer);
	void S_AuthResponse(CByteBuffer& Buffer);
    void CreateAddonsBuffer(CByteBuffer& AddonsBuffer);

private:
	CTCPSocket m_TCPSocket;
	AuthCrypt m_WoWCryptoUtils;
    std::unique_ptr<CServerPacket> m_CurrentPacket;
	std::function<bool(Opcodes, CServerPacket&)> m_ExternalHandler;

private:
	std::string m_Login;
	BigNumber   m_Key;
};

#endif