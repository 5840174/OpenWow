#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "ServerPacket.h"
#include "WorldSocket.h"

#include "Templates/CharacterTemplate.h"
#include "WoWObject.h"

// Cache
#include "ClientCache.h"

// Visible part
#include "Sky/SkyManager.h"
#include "Map/Map.h"

class ZN_API CWoWWorld
{
public:
	CWoWWorld(IScene& Scene, const std::shared_ptr<CWorldSocket>& Socket);
	virtual ~CWoWWorld();

	void EnterWorld(const CInet_CharacterTemplate& SelectedCharacter);

	void S_SMSG_LOGIN_VERIFY_WORLD(CServerPacket& Buffer);

	void On_SMSG_TIME_SYNC_REQ(CServerPacket& Buffer);

	void S_SMSG_COMPRESSED_UPDATE_OBJECT(CServerPacket & Buffer);
	void S_SMSG_UPDATE_OBJECT(CServerPacket& Buffer);
	void S_SMSG_MONSTER_MOVE(CServerPacket& Buffer);
	void S_SMSG_DESTROY_OBJECT(CServerPacket& Buffer);

	// CWoWWorld
	void AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler);
	bool ProcessHandler(Opcodes Opcode, CServerPacket& Bytes);

	void SendPacket(CClientPacket& Packet);

protected:
	void ProcessUpdatePacket(CByteBuffer& Packet);

	std::shared_ptr<WoWObject> CreateObjectByType(ObjectGuid guid, ObjectTypeID ObjectTypeID);


	std::shared_ptr<WoWObject> GetWoWObject(ObjectGuid Guid);
	bool IsWoWObjectExists(ObjectGuid Guid);

private: // Technical stuff
	std::unordered_map<Opcodes, std::function<void(CServerPacket&)>> m_Handlers;


private: // Game objects and entities
	std::shared_ptr<SkyManager> skyManager;
	std::shared_ptr<CMap> map;
	CClientCache m_Cache;
	std::unordered_map<ObjectGuid, std::shared_ptr<WoWObject>> m_WoWObjects;

private:
	IScene& m_Scene;
	std::shared_ptr<CWorldSocket> m_Socket;
};

#endif