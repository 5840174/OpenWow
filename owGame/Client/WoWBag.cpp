#include "stdafx.h"

#if 0

// General
#include "WoWBag.h"

WoWBag::WoWBag(ObjectGuid Guid)
	: WoWItem(Guid)
{
	m_ObjectType |= TYPEMASK_CONTAINER;
	m_ObjectTypeId = TYPEID_CONTAINER;
	m_valuesCount = CONTAINER_END;
}

WoWBag::~WoWBag()
{
}



//
// Public
//
std::shared_ptr<WoWBag> WoWBag::Create(IBaseManager & BaseManager, IRenderDevice & RenderDevice, IScene * Scene, ObjectGuid Guid)
{
	std::shared_ptr<WoWBag> thisObj = std::make_shared<WoWBag>(Guid);
	return thisObj;
}

#endif