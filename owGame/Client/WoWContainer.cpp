#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWContainer.h"

CWoWContainer::CWoWContainer(CWoWObjectGuid Guid)
	: WoWItem(Guid)
{
	m_ObjectType |= TYPEMASK_CONTAINER;
	m_ObjectTypeId = TYPEID_CONTAINER;
	m_valuesCount = CONTAINER_END;
}

CWoWContainer::~CWoWContainer()
{
}



//
// Public
//
std::shared_ptr<CWoWContainer> CWoWContainer::Create(IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<CWoWContainer> thisObj = std::make_shared<CWoWContainer>(Guid);
	return thisObj;
}

#endif