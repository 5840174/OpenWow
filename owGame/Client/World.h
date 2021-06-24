#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "ServerPacket.h"
#include "WorldSocket.h"

#include "Templates/CharacterTemplate.h"

#include "WorldObjects.h"
#include "WorldObjectUpdater.h"
#include "ClientCache.h"
#include "TaxiStorage.h"
#include "TransportAnimationStorage.h"

#include "Renderer/RenderPass_Path.h"

// Visible part
#include "Sky/SkyManager.h"
#include "Map/Map.h"

class ZN_API CWoWWorld
{
public:
	CWoWWorld(IScene& Scene, const std::shared_ptr<CWorldSocket>& Socket);
	virtual ~CWoWWorld();

	void EnterWorld(const CInet_CharacterTemplate& SelectedCharacter);
	void Update(const UpdateEventArgs& e);
	void Accept(IWoWVisitor * WoWVisitor);

	void S_SMSG_LOGIN_VERIFY_WORLD(CServerPacket& Buffer);
	void On_SMSG_TIME_SYNC_REQ(CServerPacket& Buffer);

	void S_SMSG_MONSTER_MOVE(CServerPacket& Buffer);
	void S_SMSG_DESTROY_OBJECT(CServerPacket& Buffer);

	// CWoWWorld
	void AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler);
	bool ProcessHandler(Opcodes Opcode, CServerPacket& Bytes);
	void SendPacket(CClientPacket& Packet);

	std::shared_ptr<CMap> GetMap() const;

	CWorldObjects& GetWorldObjects() { return m_WorldObjects; }
	CWorldObjectUpdater& GetWorldObjectUpdater() { return m_WorldObjectUpdater; }
	CClientCache& GetClientCache() { return m_ClientCache; }
	const CTaxiStorage& GetTaxiStorage() const { return m_TaxiStorage; }
	const CTransportAnimationStorage& GetTransportAnimationStorage() const { return m_TransportAnimationStorage; }

private: // Game objects and entities
	std::shared_ptr<SkyManager> m_SkyManager;
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