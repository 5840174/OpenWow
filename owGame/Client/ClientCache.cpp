#include "stdafx.h"

// General
#include "ClientCache.h"

// Additional
#include "World.h"
#include "ClientPacket.h"


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

void CClientCache::SendQueryResponce(ObjectGuid Guid)
{
	HighGuid hiGuid = Guid.GetHigh();
	ObjectGuid::LowType enGuid = Guid.GetEntry();

	switch (hiGuid)
	{
		case HighGuid::GameObject:
		{
			if (m_CacheGameObjects.find(enGuid) != m_CacheGameObjects.end())
				return;

			CClientPacket queryInfo(CMSG_GAMEOBJECT_QUERY);
			queryInfo << enGuid;
			queryInfo << Guid;
			m_World.SendPacket(queryInfo);

			// Add to cache, to prevent next requests
			m_CacheGameObjects[enGuid] = SGameObjectQueryResult();
		}
		break;

		case HighGuid::Unit:
		{
			if (m_CacheCreatures.find(enGuid) != m_CacheCreatures.end())
				return;

			CClientPacket queryInfo(CMSG_CREATURE_QUERY);
			queryInfo << enGuid;
			queryInfo << Guid;
			m_World.SendPacket(queryInfo);

			// Add to cache, to prevent next requests
			m_CacheCreatures[enGuid] = SCreatureQueryResult();
		}
		break;
	}
}

bool CClientCache::On_SMSG_GAMEOBJECT_QUERY_RESPONSE(CServerPacket& Bytes)
{
	SGameObjectQueryResult gameObjectQueryResult;
	gameObjectQueryResult.Fill(Bytes);
	gameObjectQueryResult.Print();
	m_CacheGameObjects[gameObjectQueryResult.entryID] = gameObjectQueryResult;
	return true;
}

bool CClientCache::On_SMSG_CREATURE_QUERY_RESPONSE(CServerPacket& Bytes)
{
	SCreatureQueryResult creatureQueryResult;
	creatureQueryResult.Fill(Bytes);
	creatureQueryResult.Print();
	m_CacheCreatures[creatureQueryResult.entry] = creatureQueryResult;
	return true;
}