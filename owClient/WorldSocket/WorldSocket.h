#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "../AuthSocket/AuthCrypt.h"
#include "ServerPacket.h"
#include "ClientPacket.h"

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
	: public CTCPSocket
{
public:
	CWorldSocket(const std::string& Login, BigNumber Key);
	virtual ~CWorldSocket();

	void Open(std::string Host, uint16 Port);
	void Update();

	// CWorldSocket
    void SendPacket(CClientPacket& Packet);
	void SetExternalHandler(std::function<bool(CServerPacket&)> Handler);
	
private: // Packets contructor
	void Packet1(uint16 Size, Opcodes Opcode);
	void Packet2(CByteBuffer& _buf);
	

private: // Used while connect to world
	void AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler);
	bool ProcessPacket(CServerPacket& ServerPacket);

	void On_SMSG_AUTH_CHALLENGE(CServerPacket& Buffer);
	void On_SMSG_AUTH_RESPONSE(CServerPacket& Buffer);

    void CreateAddonsBuffer(CByteBuffer& AddonsBuffer);

private:
	AuthCrypt                                     m_WoWCryptoUtils;
    std::unique_ptr<CServerPacket>                m_CurrentPacket;

	std::unordered_map<Opcodes, std::function<void(CServerPacket&)>> m_Handlers;
	std::function<bool(CServerPacket&)>  m_ExternalHandler;

private:
	std::string m_Login;
	BigNumber   m_Key;
};

#endif