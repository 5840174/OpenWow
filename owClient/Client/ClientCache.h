#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WDB/WDBCreatureCache.h"
#include "WDB/WDBGameObjectCache.h"
#include "WDB/WDBItemCache.h"

// FORWARD BEGIN
class CowServerWorld;
// FORWARD END

class CowClient_ServerQueryCache
{
public:
	CowClient_ServerQueryCache(CowServerWorld& world);
	virtual ~CowClient_ServerQueryCache();

	void RequestGameObjectTemplate(CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheGameobjectResponseListener>& Callback);
	void RequestCreatureTemplate  (CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheCreatureResponseListener>& Callback);
	void RequestItemTemplate      (CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheItemResponseListener>& Callback);

protected:
	bool On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes);
	bool On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes);
	bool On_SMSG_ITEM_QUERY_SINGLE_RESPONSE(CServerPacket& Bytes);

private:
	std::unique_ptr<CowWDBGameObjectCache>                                                             m_GameObjectsCache;
	std::map<CowGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheGameobjectResponseListener>>> m_GameObjectCallbacks;

	std::unique_ptr<CowWDBCreatureCache>                                                               m_CreaturesCache;
	std::map<CowGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheCreatureResponseListener>>>   m_CreatureCallbacks;

	std::unique_ptr<CowWDBItemCache>                                                                   m_ItemsCache;
	std::map<CowGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheItemResponseListener>>>       m_ItemsCallbacks;

	CowServerWorld& m_ServerWorld;
};

#endif
