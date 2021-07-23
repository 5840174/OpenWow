#include "stdafx.h"

// General
#include "ClientCache.h"

// Additional
#include "World/ServerWorld.h"

CowClient_Cache::CowClient_Cache(CowServerWorld& world)
	: m_ServerWorld(world)
	, m_TaxiStorage(m_ServerWorld.GetBaseManager())
	, m_TransportAnimationStorage(m_ServerWorld.GetBaseManager())
	, m_GameObjectsCache(m_ServerWorld.GetBaseManager().GetManager<IFilesManager>())
	, m_CreaturesCache(m_ServerWorld.GetBaseManager().GetManager<IFilesManager>())
	, m_ItemsCache(m_ServerWorld.GetBaseManager().GetManager<IFilesManager>())
{
	//SMSG_NAME_QUERY_RESPONSE
	//SMSG_PET_NAME_QUERY_RESPONSE
	//SMSG_GUILD_QUERY_RESPONSE
	//SMSG_PAGE_TEXT_QUERY_RESPONSE
	//SMSG_QUEST_QUERY_RESPONSE
	m_ServerWorld.AddHandler(SMSG_GAMEOBJECT_QUERY_RESPONSE, std::bind(&CowClient_Cache::On_SMSG_GAMEOBJECT_QUERY_RESPONSE, this, std::placeholders::_1));
	m_ServerWorld.AddHandler(SMSG_CREATURE_QUERY_RESPONSE, std::bind(&CowClient_Cache::On_SMSG_CREATURE_QUERY_RESPONSE, this, std::placeholders::_1));
}

CowClient_Cache::~CowClient_Cache()
{}



//
// WDB
//
void CowClient_Cache::RequestGameObjectTemplate(CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheGameobjectResponseListener>& Callback)
{
	if (Entry == 0)
		throw CException("CowClient_Cache::RequestGameObjectTemplate: Entry is 0.");

	if (Guid.GetTypeId() != EWoWObjectTypeID::TYPEID_GAMEOBJECT)
		throw CException("CowClient_Cache::RequestGameObjectTemplate: TypeID must be GameObject. %s - '%d'.", Guid.GetTypeName(), Guid.GetTypeId());

	std::shared_ptr<SGameObjectQueryResult> queryResult;
	if (m_GameObjectsCache.Get(Entry, &queryResult))
	{
		if (queryResult != nullptr)
		{
			if (Callback != nullptr)
				Callback->OnTemplate(Entry, queryResult);
		}
		else
		{
			m_GameObjectCallbacks[Entry].push_back(Callback);
		}
	}
	else
	{
		m_GameObjectCallbacks[Entry].push_back(Callback);

		CClientPacket queryInfo(CMSG_GAMEOBJECT_QUERY);
		queryInfo << Entry;
		queryInfo << Guid;
		m_ServerWorld.SendPacket(queryInfo);

		// Add to cache, to prevent next requests
		m_GameObjectsCache.Add(Entry, nullptr);
	}
}

void CowClient_Cache::RequestCreatureTemplate(CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheCreatureResponseListener>& Callback)
{
	if (Entry == 0)
		throw CException("CowClient_Cache::RequestCreatureTemplate: Entry is 0.");

	if (Guid.GetTypeId() != EWoWObjectTypeID::TYPEID_UNIT)
		throw CException("CowClient_Cache::RequestCreatureTemplate: TypeID must be Creature. %s - '%d'.", Guid.GetTypeName(), Guid.GetTypeId());

	std::shared_ptr<SCreatureQueryResult> queryResult;
	if (m_CreaturesCache.Get(Entry, &queryResult))
	{
		if (queryResult != nullptr)
		{
			if (Callback != nullptr)
				Callback->OnTemplate(Entry, queryResult);
		}
		else
		{
			m_CreatureCallbacks[Entry].push_back(Callback);
		}
	}
	else
	{
		m_CreatureCallbacks[Entry].push_back(Callback);

		CClientPacket queryInfo(CMSG_CREATURE_QUERY);
		queryInfo << Entry;
		queryInfo << Guid;
		m_ServerWorld.SendPacket(queryInfo);

		// Add to cache, to prevent next requests
		m_CreaturesCache.Add(Entry, nullptr);
	}
}

void CowClient_Cache::RequestItemTemplate(CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheItemResponseListener>& Callback)
{
	if (Entry == 0)
		throw CException("CowClient_Cache::RequestItemTemplate: Entry is 0.");

	if (Guid.GetTypeId() != EWoWObjectTypeID::TYPEID_ITEM && Guid.GetTypeId() != EWoWObjectTypeID::TYPEID_CONTAINER)
		throw CException("CowClient_Cache::RequestItemTemplate: TypeID must be Item or Container. %s - '%d'.", Guid.GetTypeName(), Guid.GetTypeId());

	std::shared_ptr<SItemQueryResult> queryResult;
	if (m_ItemsCache.Get(Entry, &queryResult))
	{
		if (queryResult != nullptr)
		{
			if (Callback != nullptr)
				Callback->OnTemplate(Entry, queryResult);
		}
		else
		{
			m_ItemsCallbacks[Entry].push_back(Callback);
		}
	}
	else
	{
		m_ItemsCallbacks[Entry].push_back(Callback);

		CClientPacket queryInfo(CMSG_ITEM_QUERY_SINGLE);
		queryInfo << Entry;
		queryInfo << Guid;
		m_ServerWorld.SendPacket(queryInfo);

		// Add to cache, to prevent next requests
		m_ItemsCache.Add(Entry, nullptr);
	}
}



//
// Protected
//
bool CowClient_Cache::On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes)
{
	uint32 entryIDWIthFlag;
	Bytes >> entryIDWIthFlag;
	if (entryIDWIthFlag & 0x80000000)
	{
		Log::Warn("CowClient_Cache: On_SMSG_GAMEOBJECT_QUERY_RESPONSE is not allowed.");
		return false;
	}

	uint32 entryIDWIthoutFlag = entryIDWIthFlag & ~(0x80000000);

	auto gameObjectQueryResult = MakeShared(SGameObjectQueryResult, entryIDWIthoutFlag);
	gameObjectQueryResult->Load(Bytes);
	m_GameObjectsCache.Add(entryIDWIthoutFlag, gameObjectQueryResult);

	// Callback
	const auto& gameObjectCallbacksIt = m_GameObjectCallbacks.find(entryIDWIthoutFlag);
	if (gameObjectCallbacksIt != m_GameObjectCallbacks.end())
	{
		for (const auto& callbackIt : gameObjectCallbacksIt->second)
			if (auto lockedCallback = callbackIt.lock())
				lockedCallback->OnTemplate(entryIDWIthoutFlag, gameObjectQueryResult);
		m_GameObjectCallbacks.erase(gameObjectCallbacksIt);
	}

	return true;
}

bool CowClient_Cache::On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes)
{
	uint32 entryIDWIthFlag;
	Bytes >> entryIDWIthFlag;
	if (entryIDWIthFlag & 0x80000000)
	{
		Log::Warn("CowClient_Cache: On_SMSG_CREATURE_QUERY_RESPONSE is not allowed.");
		return false;
	}

	uint32 entryIDWIthoutFlag = entryIDWIthFlag & ~(0x80000000);

	auto creatureQueryResult = MakeShared(SCreatureQueryResult, entryIDWIthoutFlag);
	creatureQueryResult->Load(Bytes);
	m_CreaturesCache.Add(entryIDWIthoutFlag, creatureQueryResult);

	// Callback
	const auto& creatureCallbacksIt = m_CreatureCallbacks.find(entryIDWIthoutFlag);
	if (creatureCallbacksIt != m_CreatureCallbacks.end())
	{
		for (const auto& callbackIt : creatureCallbacksIt->second)
			if (auto lockedCallback = callbackIt.lock())
				lockedCallback->OnTemplate(entryIDWIthoutFlag, creatureQueryResult);
		m_CreatureCallbacks.erase(creatureCallbacksIt);
	}

	return true;
}

bool CowClient_Cache::On_SMSG_ITEM_QUERY_SINGLE_RESPONSE(CServerPacket & Bytes)
{
	uint32 entryIDWIthFlag;
	Bytes >> entryIDWIthFlag;
	if (entryIDWIthFlag & 0x80000000)
	{
		Log::Warn("CowClient_Cache: On_SMSG_ITEM_QUERY_SINGLE_RESPONSE is not allowed.");
		return false;
	}

	uint32 entryIDWIthoutFlag = entryIDWIthFlag & ~(0x80000000);

	auto itemQueryResult = MakeShared(SItemQueryResult, entryIDWIthoutFlag);
	itemQueryResult->Load(Bytes);
	m_ItemsCache.Add(entryIDWIthoutFlag, itemQueryResult);

	// Callback
	const auto& itemCallbacksIt = m_ItemsCallbacks.find(entryIDWIthoutFlag);
	if (itemCallbacksIt != m_ItemsCallbacks.end())
	{
		for (const auto& callbackIt : itemCallbacksIt->second)
			if (auto lockedCallback = callbackIt.lock())
				lockedCallback->OnTemplate(entryIDWIthoutFlag, itemQueryResult);
		m_ItemsCallbacks.erase(itemCallbacksIt);
	}

	return true;
}
