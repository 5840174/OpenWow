#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWPlayer.h"

WoWPlayer::WoWPlayer(IScene& Scene, CWoWObjectGuid Guid)
	: WoWUnit(Scene, Guid)
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
std::shared_ptr<WoWPlayer> WoWPlayer::Create(IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWPlayer> thisObj = MakeShared(WoWPlayer, Scene, Guid);
	return thisObj;
}

#endif
