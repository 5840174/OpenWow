#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "../AuthSocket/AuthCrypt.h"
#include "PacketsQueue.h"

#include "../Warden/Warden.h"

// FORWARD BEGIN
class CowClient;
// FORWARD END

class CWorldSocket
	: public CTCPSocket
{
public:
	CWorldSocket(const std::string& Login, BigNumber Key);
	virtual ~CWorldSocket();

	void Open(std::string Host, uint16 Port);
	void Update();
	void UpdateFromThread(std::future<void> PromiseExiter);

	// CWorldSocket
	void OnConnected() override;
	void OnDisconnected() override;
    void SendPacket(CClientPacket& Packet);
	void SetExternalHandler(std::function<bool(CServerPacket&)> Handler);
		
private: // Used while connect to world
	void AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler);
	bool ProcessPacket(CServerPacket& ServerPacket);

	void On_SMSG_AUTH_CHALLENGE(CServerPacket& Buffer);
	void On_SMSG_AUTH_RESPONSE(CServerPacket& Buffer);

	void On_SMSG_WARDEN_DATA(CServerPacket& Buffer);

    void CreateAddonsBuffer(CByteBuffer& AddonsBuffer);

private:
	std::thread                                   m_UpdateThread;
	std::promise<void>					          m_UpdateThreadExiter;

private:
	AuthCrypt                                     m_WoWCryptoUtils;
	CPacketsQueue                                 m_PacketsQueue;

	std::unordered_map<Opcodes, std::function<void(CServerPacket&)>> m_Handlers;
	std::function<bool(CServerPacket&)> m_ExternalHandler;

private:
	std::string m_Login;
	BigNumber   m_Key;
	std::unique_ptr<CWarden> m_Warden;
};

#endif