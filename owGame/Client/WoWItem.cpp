#include "stdafx.h"

// General
#include "WoWItem.h"

WoWItem::WoWItem()
{
}

WoWItem::~WoWItem()
{
}

std::shared_ptr<WoWItem> WoWItem::Create(IBaseManager & BaseManager, IRenderDevice & RenderDevice, IScene * Scene, uint64 guid)
{
	std::shared_ptr<WoWItem> thisObj = std::make_shared<WoWItem>();
	thisObj->InitInternal(guid, TYPEMASK_ITEM, ObjectTypeID::TYPEID_ITEM);
	thisObj->m_valuesCount = ITEM_END;
	return thisObj;
}