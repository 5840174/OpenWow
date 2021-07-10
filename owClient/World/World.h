#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "../WorldSocket/WorldSocket.h"

#include "WorldObjects.h"
#include "WorldObjectUpdater.h"

#include "../Client/ClientCache.h"
#include "../Client/TaxiStorage.h"
#include "../Client/TransportAnimationStorage.h"

class ZN_API CWoWWorld
{
public:
	CWoWWorld(IScene& Scene, const std::shared_ptr<CWorldSocket>& Socket);
	virtual ~CWoWWorld();

	void EnterWorld(const SCharacterTemplate& SelectedCharacter);
	void Update(const UpdateEventArgs& e);
	void Accept(IWoWVisitor * WoWVisitor);

	void S_SMSG_LOGIN_VERIFY_WORLD(CServerPacket& Buffer);
	
	void On_SMSG_MOTD(CServerPacket& Buffer);
	void On_SMSG_NOTIFICATION(CServerPacket& Buffer);

	void On_SMSG_SPELL_GO(CServerPacket & Buffer);

	void On_SMSG_POWER_UPDATE(CServerPacket& Buffer);

	void On_SMSG_AURA_UPDATE(CServerPacket& Buffer);
	void On_SMSG_AURA_UPDATE_ALL(CServerPacket& Buffer);
	void Do_AuraUpdate(CServerPacket& Buffer);

	void On_SMSG_UPDATE_WORLD_STATE(CServerPacket& Buffer);
	void On_SMSG_TIME_SYNC_REQ(CServerPacket& Buffer);

	void S_SMSG_MONSTER_MOVE(CServerPacket& Buffer, bool Transport);
	void S_SMSG_DESTROY_OBJECT(CServerPacket& Buffer);

	void On_SMSG_EMOTE(CServerPacket& Buffer);
	void On_SMSG_MESSAGECHAT(CServerPacket& Buffer, bool IsGMMessage);

	void On_MOVE_Opcode(CServerPacket& Buffer);
	void On_MSG_MOVE_TIME_SKIPPED(CServerPacket& Buffer);

	void On_MOVE_UnitSpeedOpcode(CServerPacket& Buffer);

	// CWoWWorld
	void AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler);
	bool ProcessPacket(CServerPacket& ServerPacket);
	void SendPacket(CClientPacket& Packet);

	std::shared_ptr<CMap> GetMap() const;

	CWorldObjects& GetWorldObjects() { return m_WorldObjects; }
	CWorldObjectUpdater& GetWorldObjectUpdater() { return m_WorldObjectUpdater; }
	CClientCache& GetClientCache() { return m_ClientCache; }
	const CTaxiStorage& GetTaxiStorage() const { return m_TaxiStorage; }
	const CTransportAnimationStorage& GetTransportAnimationStorage() const { return m_TransportAnimationStorage; }

private: // Game objects and entities
	std::shared_ptr<CSkyManager> m_SkyManager;
	std::shared_ptr<CMap> m_Map;
	
private:
	IScene& m_Scene;
	std::weak_ptr<CWorldSocket> m_Socket;
	std::unordered_map<Opcodes, std::function<void(CServerPacket&)>> m_Handlers;

	CWorldObjects m_WorldObjects;
	CWorldObjectUpdater m_WorldObjectUpdater;
	CClientCache m_ClientCache;
	CTaxiStorage m_TaxiStorage;
	CTransportAnimationStorage m_TransportAnimationStorage;
};

#endif