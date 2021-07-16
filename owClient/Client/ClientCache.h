#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "ClientCacheTemplates.h"

// FORWARD BEGIN
class CowServerWorld;
// FORWARD END

class CowClient_ServerQueryCache
{
public:
	CowClient_ServerQueryCache(CowServerWorld& world);

	void SendGameObjectQueryResponce(CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheGameobjectResponseListener>& Callback);
	void SendCreatureQueryResponce  (CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheCreatureResponseListener>& Callback);

	bool On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes);
	bool On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes);

private:
	std::map<CowGuid::EntryType_t, std::shared_ptr<SGameObjectQueryResult>>                            m_CacheGameObjects;
	std::map<CowGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheGameobjectResponseListener>>> m_GameObjectCallbacks;

	std::map<CowGuid::EntryType_t, std::shared_ptr<SCreatureQueryResult>>                              m_CacheCreatures;
	std::map<CowGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheCreatureResponseListener>>>   m_CreatureCallbacks;

	CowServerWorld& m_ServerWorld;
};

#endif
