#include "stdafx.h"

// General
#include "ClientCache.h"

// Additional
#include "World/World.h"
#include "WorldSocket/ClientPacket.h"


CClientCache::CClientCache(CWoWWorld & world)
	: m_World(world)
{
	//SMSG_NAME_QUERY_RESPONSE
	//SMSG_PET_NAME_QUERY_RESPONSE
	//SMSG_GUILD_QUERY_RESPONSE
	//SMSG_ITEM_QUERY_SINGLE_RESPONSE
	//SMSG_ITEM_QUERY_MULTIPLE_RESPONSE
	//SMSG_PAGE_TEXT_QUERY_RESPONSE
	//SMSG_QUEST_QUERY_RESPONSE
	m_World.AddHandler(SMSG_GAMEOBJECT_QUERY_RESPONSE, std::bind(&CClientCache::On_SMSG_GAMEOBJECT_QUERY_RESPONSE, this, std::placeholders::_1));
	m_World.AddHandler(SMSG_CREATURE_QUERY_RESPONSE, std::bind(&CClientCache::On_SMSG_CREATURE_QUERY_RESPONSE, this, std::placeholders::_1));
}

void CClientCache::SendQueryResponce(CWoWObjectGuid::EntryType_t Entry, CWoWObjectGuid Guid)
{
	EWoWObjectTypeID typeID = Guid.GetTypeId();
	if (Entry == 0)
		throw CException("CClientCache::SendQueryResponce: Entry is 0.");

	switch (typeID)
	{
		case EWoWObjectTypeID::TYPEID_GAMEOBJECT:
		{
			const auto& cacheGameObjectsIt = m_CacheGameObjects.find(Entry);
			if (cacheGameObjectsIt != m_CacheGameObjects.end())
			{
				// Callback
				const auto& gameObjectCallbacksIt = m_GameObjectCallbacks.find(Entry);
				if (gameObjectCallbacksIt != m_GameObjectCallbacks.end())
				{
					for (const auto& callbackIt : gameObjectCallbacksIt->second)
						if (callbackIt != nullptr)
							callbackIt(Entry, cacheGameObjectsIt->second);
					m_GameObjectCallbacks.erase(gameObjectCallbacksIt);
				}

				return;
			}

			CClientPacket queryInfo(CMSG_GAMEOBJECT_QUERY);
			queryInfo << Entry;
			queryInfo << Guid;
			m_World.SendPacket(queryInfo);

			// Add to cache, to prevent next requests
			m_CacheGameObjects[Entry] = nullptr;
		}
		break;

		case EWoWObjectTypeID::TYPEID_UNIT:
		{
			if (m_CacheCreatures.find(Entry) != m_CacheCreatures.end())
				return;

			CClientPacket queryInfo(CMSG_CREATURE_QUERY);
			queryInfo << Entry;
			queryInfo << Guid;
			m_World.SendPacket(queryInfo);

			// Add to cache, to prevent next requests
			m_CacheCreatures[Entry] = nullptr;
		}
		break;
	}
}

void CClientCache::SendQueryResponceWithCallback(CWoWObjectGuid::EntryType_t Entry, CWoWObjectGuid Guid, std::function<void(CWoWObjectGuid::EntryType_t, const std::shared_ptr<SGameObjectQueryResult>&)> OnGameObjectQueryResponceReceived)
{
	SendQueryResponce(Entry, Guid);
	m_GameObjectCallbacks[Entry].push_back(OnGameObjectQueryResponceReceived);
}

bool CClientCache::On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes)
{
	uint32 entryIDWIthFlag;
	Bytes.read(&entryIDWIthFlag);
	uint32 entryIDWIthoutFlag = entryIDWIthFlag & ~(0x80000000);

	//const auto& cacheGameObjectsIt = m_CacheGameObjects.find(entryIDWIthoutFlag);
	//if (cacheGameObjectsIt == m_CacheGameObjects.end())
	//	throw CException("Unexpected behaviour");

	if (entryIDWIthFlag & 0x80000000)
	{
		Log::Warn("CClientCache: On_SMSG_GAMEOBJECT_QUERY_RESPONSE is not allowed.");
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
			if (callbackIt != nullptr)
				callbackIt(entryIDWIthoutFlag, gameObjectQueryResult);
		m_GameObjectCallbacks.erase(gameObjectCallbacksIt);
	}

	return true;
}

bool CClientCache::On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes)
{
	uint32 entryIDWIthFlag;
	Bytes.read(&entryIDWIthFlag);
	uint32 entryIDWIthoutFlag = entryIDWIthFlag & ~(0x80000000);

	if (entryIDWIthFlag & 0x80000000)
	{
		Log::Warn("CClientCache: On_SMSG_CREATURE_QUERY_RESPONSE is not allowed.");
		return false;
	}

	std::shared_ptr<SCreatureQueryResult> creatureQueryResult = MakeShared(SCreatureQueryResult, entryIDWIthoutFlag);
	creatureQueryResult->Fill(Bytes);
	creatureQueryResult->Print();
	m_CacheCreatures[creatureQueryResult->entry] = creatureQueryResult;
	return true;
}