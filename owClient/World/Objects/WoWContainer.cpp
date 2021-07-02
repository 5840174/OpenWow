#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWContainer.h"

CWoWContainer::CWoWContainer(CWoWGuid Guid)
	: WoWItem(Guid)
{
	m_ObjectType |= TYPEMASK_CONTAINER;
	m_Values.SetValuesCount(CONTAINER_END);
}

CWoWContainer::~CWoWContainer()
{
}



//
// Public
//
std::shared_ptr<CWoWContainer> CWoWContainer::Create(IScene& Scene, CWoWGuid Guid)
{
	std::shared_ptr<CWoWContainer> thisObj = MakeShared(CWoWContainer, Guid);
	return thisObj;
}

#endif