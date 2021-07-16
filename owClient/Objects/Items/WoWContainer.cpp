#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWContainer.h"

CowServerContainer::CowServerContainer(CowGuid Guid)
	: CowServerItem(Guid)
{
	//m_ObjectType |= TYPEMASK_CONTAINER;
	m_Values.SetValuesCount(CONTAINER_END);
}

CowServerContainer::~CowServerContainer()
{
}



//
// Public
//
std::shared_ptr<CowServerContainer> CowServerContainer::Create(IScene& Scene, CowGuid Guid)
{
	std::shared_ptr<CowServerContainer> thisObj = MakeShared(CowServerContainer, Guid);
	return thisObj;
}

#endif