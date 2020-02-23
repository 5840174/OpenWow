#include "stdafx.h"

// Include
#include "Client.h"

// General
#include "ClientCache.h"

// Additional
#include "Client/ClientPacket.h"
#include "ObjectDefines.h"

/*
	StoreOpcode(CMSG_NAME_QUERY                                                , "CMSG_NAME_QUERY"                                                 , STATUS_LOGGEDIN              , &WorldSession::HandleNameQueryOpcode);
	StoreOpcode(SMSG_NAME_QUERY_RESPONSE                                       , "SMSG_NAME_QUERY_RESPONSE"                                        , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
	StoreOpcode(CMSG_PET_NAME_QUERY                                            , "CMSG_PET_NAME_QUERY"                                             , STATUS_LOGGEDIN              , &WorldSession::HandlePetNameQuery);
	StoreOpcode(SMSG_PET_NAME_QUERY_RESPONSE                                   , "SMSG_PET_NAME_QUERY_RESPONSE"                                    , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
	StoreOpcode(CMSG_GUILD_QUERY                                               , "CMSG_GUILD_QUERY"                                                , STATUS_AUTHED                , &WorldSession::HandleGuildQueryOpcode);
	StoreOpcode(SMSG_GUILD_QUERY_RESPONSE                                      , "SMSG_GUILD_QUERY_RESPONSE"                                       , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
	StoreOpcode(CMSG_ITEM_QUERY_SINGLE                                         , "CMSG_ITEM_QUERY_SINGLE"                                          , STATUS_LOGGEDIN              , &WorldSession::HandleItemQuerySingleOpcode);
	StoreOpcode(CMSG_ITEM_QUERY_MULTIPLE                                       , "CMSG_ITEM_QUERY_MULTIPLE"                                        , STATUS_NEVER                 , &WorldSession::Handle_NULL);
	StoreOpcode(SMSG_ITEM_QUERY_SINGLE_RESPONSE                                , "SMSG_ITEM_QUERY_SINGLE_RESPONSE"                                 , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
	StoreOpcode(SMSG_ITEM_QUERY_MULTIPLE_RESPONSE                              , "SMSG_ITEM_QUERY_MULTIPLE_RESPONSE"                               , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
	StoreOpcode(CMSG_PAGE_TEXT_QUERY                                           , "CMSG_PAGE_TEXT_QUERY"                                            , STATUS_LOGGEDIN              , &WorldSession::HandlePageQueryOpcode);
	StoreOpcode(SMSG_PAGE_TEXT_QUERY_RESPONSE                                  , "SMSG_PAGE_TEXT_QUERY_RESPONSE"                                   , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
	StoreOpcode(CMSG_QUEST_QUERY                                               , "CMSG_QUEST_QUERY"                                                , STATUS_LOGGEDIN              , &WorldSession::HandleQuestQueryOpcode);
	StoreOpcode(SMSG_QUEST_QUERY_RESPONSE                                      , "SMSG_QUEST_QUERY_RESPONSE"                                       , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
	StoreOpcode(CMSG_GAMEOBJECT_QUERY                                          , "CMSG_GAMEOBJECT_QUERY"                                           , STATUS_LOGGEDIN              , &WorldSession::HandleGameObjectQueryOpcode);
	StoreOpcode(SMSG_GAMEOBJECT_QUERY_RESPONSE                                 , "SMSG_GAMEOBJECT_QUERY_RESPONSE"                                  , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
	StoreOpcode(CMSG_CREATURE_QUERY                                            , "CMSG_CREATURE_QUERY"                                             , STATUS_LOGGEDIN              , &WorldSession::HandleCreatureQueryOpcode);
	StoreOpcode(SMSG_CREATURE_QUERY_RESPONSE                                   , "SMSG_CREATURE_QUERY_RESPONSE"                                    , STATUS_NEVER                 , &WorldSession::Handle_ServerSide);
*/

CWoWClientCache::CWoWClientCache(const CWoWClient& WoWClient)
	: m_WoWClient(WoWClient)
{
}

CWoWClientCache::~CWoWClientCache()
{
}

void CWoWClientCache::SendQueryResponce(uint64 guid)
{
	uint32 hiGuid = GUID_HIPART(guid);
	uint32 enGuid = GUID_ENPART(guid);

	switch (hiGuid)
	{
		case HIGHGUID_GAMEOBJECT:
		{
			if (m_GameObjects.find(enGuid) != m_GameObjects.end())
				return;

			CClientPacket queryInfo(CMSG_GAMEOBJECT_QUERY);
			queryInfo << enGuid;
			queryInfo << guid;
			m_WoWClient.SendPacket(queryInfo);

			// Add to cache, to prevent next requests
			m_GameObjects[enGuid] = SGameObjectQueryResult();
		}
		break;

		case HIGHGUID_UNIT:
		{
			if (m_Creatures.find(enGuid) != m_Creatures.end())
				return;

			CClientPacket queryInfo(CMSG_CREATURE_QUERY);
			queryInfo << enGuid;
			queryInfo << guid;
			m_WoWClient.SendPacket(queryInfo);

			// Add to cache, to prevent next requests
			m_Creatures[enGuid] = SCreatureQueryResult();
		}
		break;
	}
}

bool CWoWClientCache::ProcessQueryResponse(Opcodes Opcode, CByteBuffer & Bytes)
{
	switch (Opcode)
	{
		case SMSG_NAME_QUERY_RESPONSE:
		{

		}
		break;
		case SMSG_PET_NAME_QUERY_RESPONSE:
		{

		}
		break;
		case SMSG_GUILD_QUERY_RESPONSE:
		{

		}
		break;
		case SMSG_ITEM_QUERY_SINGLE_RESPONSE:
		{

		}
		break;
		case SMSG_ITEM_QUERY_MULTIPLE_RESPONSE:
		{

		}
		break;
		case SMSG_PAGE_TEXT_QUERY_RESPONSE:
		{

		}
		break;
		case SMSG_QUEST_QUERY_RESPONSE:
		{

		}
		break;
		case SMSG_GAMEOBJECT_QUERY_RESPONSE:
		{
			SGameObjectQueryResult gameObjectQueryResult;
			gameObjectQueryResult.Fill(Bytes);
			gameObjectQueryResult.Print();
			m_GameObjects[gameObjectQueryResult.entryID] = gameObjectQueryResult;
			return true;
		}
		break;
		case SMSG_CREATURE_QUERY_RESPONSE:
		{
			SCreatureQueryResult creatureQueryResult;
			creatureQueryResult.Fill(Bytes);
			creatureQueryResult.Print();
			m_Creatures[creatureQueryResult.entry] = creatureQueryResult;
			return true;
		}
		break;
	}

	return false;
}
