#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWCorpse.h"

WoWCorpse::WoWCorpse(IScene& Scene, CWoWObjectGuid Guid)
	: CWoWWorldObject(Scene, Guid)
{
	m_ObjectType |= TYPEMASK_CORPSE;
	m_ObjectTypeId = TYPEID_CORPSE;
	m_valuesCount = CORPSE_END;
}

WoWCorpse::~WoWCorpse()
{
}

std::shared_ptr<WoWCorpse> WoWCorpse::Create(IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWCorpse> thisObj = MakeShared(WoWCorpse, Scene, Guid);
	return thisObj;
}

#endif