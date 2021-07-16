#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWCorpse.h"

// Additional
#include "../World/ServerWorld.h"

CowServerCorpse::CowServerCorpse(IScene& Scene, CowServerWorld& WoWWorld, CowGuid Guid)
	: CowServerWorldObject(Scene, WoWWorld, Guid)
{
	//m_ObjectType |= TYPEMASK_CORPSE;
	m_Values.SetValuesCount(CORPSE_END);
}

CowServerCorpse::~CowServerCorpse()
{
}

std::shared_ptr<CowServerCorpse> CowServerCorpse::Create(CowServerWorld& WoWWorld, IScene& Scene, CowGuid Guid)
{
	std::shared_ptr<CowServerCorpse> thisObj = MakeShared(CowServerCorpse, Scene, WoWWorld, Guid);
	return thisObj;
}

#endif