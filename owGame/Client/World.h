#pragma once

#include "ServerPacket.h"
#include "WorldSocket.h"

#include "Object.h"

// Cache
#include "ClientCache.h"

// Visible part
#include "Sky/SkyManager.h"
#include "Map/Map.h"

class ZN_API WoWWorld
{
public:
	WoWWorld(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene* Scene, const std::shared_ptr<CWorldSocket>& Socket);
	virtual ~WoWWorld();

	void S_SMSG_CHAR_ENUM(CServerPacket& Bytes);
	void S_SMSG_LOGIN_VERIFY_WORLD(CServerPacket& Buffer);
	void S_SMSG_COMPRESSED_UPDATE_OBJECT(CServerPacket & Buffer);
	void S_SMSG_UPDATE_OBJECT(CServerPacket& Buffer);
	void S_SMSG_MONSTER_MOVE(CServerPacket& Buffer);

	// WoWWorld
	void AddHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler);
	void SendPacket(CClientPacket& Packet);
	bool ProcessHandler(Opcodes Opcode, CServerPacket& Bytes);

protected:
	void ProcessUpdatePacket(CByteBuffer& Packet);

	std::shared_ptr<WoWObject> CreateObjectByType(uint64 guid, ObjectTypeID ObjectTypeID);
	std::shared_ptr<WoWObject> GetWoWObject(uint64 Guid);

protected: // Cache
	void SendQueryResponce(uint64 guid);
	bool ProcessQueryResponse(Opcodes Opcode, CServerPacket& Bytes);


private: // Technical stuff
	std::shared_ptr<CWorldSocket> m_Socket;
	std::unordered_map<Opcodes, std::function<void(CServerPacket&)>> m_Handlers;


private: // Game objects and entities
	
	std::unordered_map<uint64, std::shared_ptr<WoWObject>> m_Objects;


private: // Cache
	std::unordered_map<uint32, SGameObjectQueryResult> m_CacheGameObjects;
	std::unordered_map<uint32, SCreatureQueryResult> m_CacheCreatures;

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	IScene* m_Scene;
	std::shared_ptr<SkyManager> skyManager;
	std::shared_ptr<CMap> map;
};