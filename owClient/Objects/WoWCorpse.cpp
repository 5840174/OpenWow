#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWCorpse.h"

// Additional
#include "../World/World.h"

WoWCorpse::WoWCorpse(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid)
	: CWoWWorldObject(Scene, WoWWorld, Guid)
{
	//m_ObjectType |= TYPEMASK_CORPSE;
	m_Values.SetValuesCount(CORPSE_END);
}

WoWCorpse::~WoWCorpse()
{
}

std::shared_ptr<WoWCorpse> WoWCorpse::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid)
{
	std::shared_ptr<WoWCorpse> thisObj = MakeShared(WoWCorpse, Scene, WoWWorld, Guid);
	return thisObj;
}

#endif