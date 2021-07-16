#include "stdafx.h"

// General
#include "ClientCache.h"

// Additional
#include "World/ServerWorld.h"

CowClient_ServerQueryCache::CowClient_ServerQueryCache(CowServerWorld & world)
	: m_ServerWorld(world)
{
	//SMSG_NAME_QUERY_RESPONSE
	//SMSG_PET_NAME_QUERY_RESPONSE
	//SMSG_GUILD_QUERY_RESPONSE
	//SMSG_ITEM_QUERY_SINGLE_RESPONSE
	//SMSG_ITEM_QUERY_MULTIPLE_RESPONSE
	//SMSG_PAGE_TEXT_QUERY_RESPONSE
	//SMSG_QUEST_QUERY_RESPONSE
	m_ServerWorld.AddHandler(SMSG_GAMEOBJECT_QUERY_RESPONSE, std::bind(&CowClient_ServerQueryCache::On_SMSG_GAMEOBJECT_QUERY_RESPONSE, this, std::placeholders::_1));
	m_ServerWorld.AddHandler(SMSG_CREATURE_QUERY_RESPONSE, std::bind(&CowClient_ServerQueryCache::On_SMSG_CREATURE_QUERY_RESPONSE, this, std::placeholders::_1));
}

void CowClient_ServerQueryCache::SendGameObjectQueryResponce(CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheGameobjectResponseListener>& Callback)
{
	if (Entry == 0)
		throw CException("CowClient_ServerQueryCache::SendGameObjectQueryResponce: Entry is 0.");

	if (Guid.GetTypeId() != EWoWObjectTypeID::TYPEID_GAMEOBJECT)
		throw CException("CowClient_ServerQueryCache::SendGameObjectQueryResponce: TypeID must be GameObject. %s - '%d'.", Guid.GetTypeName(), Guid.GetTypeId());

	const auto& cacheGameObjectsIt = m_CacheGameObjects.find(Entry);
	if (cacheGameObjectsIt != m_CacheGameObjects.end())
	{
		if (cacheGameObjectsIt->second != nullptr)
		{
			if (Callback != nullptr && cacheGameObjectsIt->second != nullptr)
			{
				Callback->OnTemplate(Entry, cacheGameObjectsIt->second);
			}
		}
		else
		{
			m_GameObjectCallbacks[Entry].push_back(Callback);
		}

		return;
	}
	else
	{
		m_GameObjectCallbacks[Entry].push_back(Callback);
	}

	CClientPacket queryInfo(CMSG_GAMEOBJECT_QUERY);
	queryInfo << Entry;
	queryInfo << Guid;
	m_ServerWorld.SendPacket(queryInfo);

	// Add to cache, to prevent next requests
	m_CacheGameObjects[Entry] = nullptr;
}

void CowClient_ServerQueryCache::SendCreatureQueryResponce(CowGuid::EntryType_t Entry, CowGuid Guid, const std::shared_ptr<IClientCacheCreatureResponseListener>& Callback)
{
	if (Entry == 0)
		throw CException("CowClient_ServerQueryCache::SendCreatureQueryResponce: Entry is 0.");

	if (Guid.GetTypeId() != EWoWObjectTypeID::TYPEID_UNIT)
		throw CException("CowClient_ServerQueryCache::SendCreatureQueryResponce: TypeID must be Creature. %s - '%d'.", Guid.GetTypeName(), Guid.GetTypeId());

	const auto& cacheCreatureIt = m_CacheCreatures.find(Entry);
	if (cacheCreatureIt != m_CacheCreatures.end())
	{
		if (cacheCreatureIt->second != nullptr)
		{
			if (Callback != nullptr)
			{
				Callback->OnTemplate(Entry, cacheCreatureIt->second);
			}
		}
		else
		{
			m_CreatureCallbacks[Entry].push_back(Callback);
		}

		return;
	}
	else
	{
		m_CreatureCallbacks[Entry].push_back(Callback);
	}

	CClientPacket queryInfo(CMSG_CREATURE_QUERY);
	queryInfo << Entry;
	queryInfo << Guid;
	m_ServerWorld.SendPacket(queryInfo);

	// Add to cache, to prevent next requests
	m_CacheCreatures[Entry] = nullptr;
}

bool CowClient_ServerQueryCache::On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes)
{
	uint32 entryIDWIthFlag;
	Bytes >> entryIDWIthFlag;
	uint32 entryIDWIthoutFlag = entryIDWIthFlag & ~(0x80000000);

	if (entryIDWIthFlag & 0x80000000)
	{
		Log::Warn("CowClient_ServerQueryCache: On_SMSG_GAMEOBJECT_QUERY_RESPONSE is not allowed.");
		return false;
	}

	std::shared_ptr<SGameObjectQueryResult> gameObjectQueryResult = MakeShared(SGameObjectQueryResult, entryIDWIthoutFlag);
	gameObjectQueryResult->Fill(Bytes);
	gameObjectQueryResult->Print();
	m_CacheGameObjects[gameObjectQueryResult->entryID] = gameObjectQueryResult;

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

bool CowClient_ServerQueryCache::On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes)
{
	uint32 entryIDWIthFlag;
	Bytes >> entryIDWIthFlag;
	uint32 entryIDWIthoutFlag = entryIDWIthFlag & ~(0x80000000);

	if (entryIDWIthFlag & 0x80000000)
	{
		Log::Warn("CowClient_ServerQueryCache: On_SMSG_CREATURE_QUERY_RESPONSE is not allowed.");
		return false;
	}

	std::shared_ptr<SCreatureQueryResult> creatureQueryResult = MakeShared(SCreatureQueryResult, entryIDWIthoutFlag);
	creatureQueryResult->Fill(Bytes);
	creatureQueryResult->Print();
	m_CacheCreatures[creatureQueryResult->entry] = creatureQueryResult;

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