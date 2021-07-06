#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "ClientCacheTemplates.h"

// FORWARD BEGIN
class CWoWWorld;
// FORWARD END

class CClientCache
{
public:
	CClientCache(CWoWWorld& world);

	void SendGameObjectQueryResponce(CWoWGuid::EntryType_t Entry, CWoWGuid Guid, const std::shared_ptr<IClientCacheGameobjectResponseListener>& Callback);
	void SendCreatureQueryResponce  (CWoWGuid::EntryType_t Entry, CWoWGuid Guid, const std::shared_ptr<IClientCacheCreatureResponseListener>& Callback);

	bool On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes);
	bool On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes);

private:
	std::map<CWoWGuid::EntryType_t, std::shared_ptr<SGameObjectQueryResult>>                            m_CacheGameObjects;
	std::map<CWoWGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheGameobjectResponseListener>>> m_GameObjectCallbacks;

	std::map<CWoWGuid::EntryType_t, std::shared_ptr<SCreatureQueryResult>>                              m_CacheCreatures;
	std::map<CWoWGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheCreatureResponseListener>>>   m_CreatureCallbacks;

	CWoWWorld& m_World;
};

#endif
