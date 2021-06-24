#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWPlayer.h"

// Additional
#include "World.h"

WoWPlayer::WoWPlayer(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid)
	: WoWUnit(Scene, WoWWorld, Guid)
{
	m_ObjectType |= TYPEMASK_PLAYER;
	m_ObjectTypeId = TYPEID_PLAYER;
	m_valuesCount = PLAYER_END;
}

WoWPlayer::~WoWPlayer()
{
}



//
// Protected
//
std::shared_ptr<WoWPlayer> WoWPlayer::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWPlayer> thisObj = MakeShared(WoWPlayer, Scene, WoWWorld, Guid);
	return thisObj;
}

#endif
