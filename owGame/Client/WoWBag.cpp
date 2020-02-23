#include "stdafx.h"

// General
#include "WoWBag.h"

WoWBag::WoWBag()
{
}

WoWBag::~WoWBag()
{
}



//
// Public
//
std::shared_ptr<WoWBag> WoWBag::Create(IBaseManager & BaseManager, IRenderDevice & RenderDevice, IScene * Scene, uint64 guid)
{
	std::shared_ptr<WoWBag> thisObj = std::make_shared<WoWBag>();
	thisObj->InitInternal(guid, TYPEMASK_CONTAINER, ObjectTypeID::TYPEID_CONTAINER);
	thisObj->m_valuesCount = CONTAINER_END;
	return thisObj;
}
