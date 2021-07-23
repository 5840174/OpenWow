#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "TaxiStorage.h"
#include "TransportAnimationStorage.h"

#include "WDB/WDBCreatureCache.h"
#include "WDB/WDBGameObjectCache.h"
#include "WDB/WDBItemCache.h"

// FORWARD BEGIN
class CowServerWorld;
// FORWARD END

class CowClient_Cache
{
public:
	CowClient_Cache(CowServerWorld& world);
	virtual ~CowClient_Cache();

	// DBC
	const CowClient_DBCTaxi&                        GetTaxiStorage() const { return m_TaxiStorage; }
	const CowClient_DBCTransport&                   GetTransportAnimationStorage() const { return m_TransportAnimationStorage; }

	// WDB
	void                                            RequestGameObjectTemplate(CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheGameobjectResponseListener>& Callback);
	void                                            RequestCreatureTemplate  (CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheCreatureResponseListener>& Callback);
	void                                            RequestItemTemplate      (CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheItemResponseListener>& Callback);

protected:
	bool                                            On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes);
	bool                                            On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes);
	bool                                            On_SMSG_ITEM_QUERY_SINGLE_RESPONSE(CServerPacket& Bytes);

private:
	CowServerWorld&                                                                                    m_ServerWorld;

	// DBC
	CowClient_DBCTaxi                                                                                  m_TaxiStorage;
	CowClient_DBCTransport                                                                             m_TransportAnimationStorage;

	// WDB
	CowWDBGameObjectCache                                                                              m_GameObjectsCache;
	std::map<CowGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheGameobjectResponseListener>>> m_GameObjectCallbacks;
	CowWDBCreatureCache                                                                                m_CreaturesCache;
	std::map<CowGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheCreatureResponseListener>>>   m_CreatureCallbacks;
	CowWDBItemCache                                                                                    m_ItemsCache;
	std::map<CowGuid::EntryType_t, std::vector<std::weak_ptr<IClientCacheItemResponseListener>>>       m_ItemsCallbacks;
};

#endif
