#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "../WorldSocket/ServerPacket.h"

#include "ClientCacheTemplates.h"

// FORWARD BEGIN
class CWoWWorld;
// FORWARD END

class CClientCache
{
public:
	CClientCache(CWoWWorld& world);

	void SendQueryResponce            (CWoWObjectGuid::EntryType_t Entry, CWoWObjectGuid Guid);
	void SendQueryResponceWithCallback(CWoWObjectGuid::EntryType_t Entry, CWoWObjectGuid Guid, std::function<void(CWoWObjectGuid::EntryType_t, const std::shared_ptr<SGameObjectQueryResult>&)> OnGameObjectQueryResponceReceived);

	bool On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes);
	bool On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes);

private:
	std::map<CWoWObjectGuid::EntryType_t, std::shared_ptr<SGameObjectQueryResult>> m_CacheGameObjects;
	std::map<CWoWObjectGuid::EntryType_t, std::vector<std::function<void(CWoWObjectGuid::EntryType_t, const std::shared_ptr<SGameObjectQueryResult>&)>>> m_GameObjectCallbacks;

	std::map<CWoWObjectGuid::EntryType_t, std::shared_ptr<SCreatureQueryResult>> m_CacheCreatures;



	CWoWWorld& m_World;
};

#endif
