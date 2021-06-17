#include "stdafx.h"

#if 0

// General
#include "WoWItem.h"

WoWItem::WoWItem(ObjectGuid Guid)
	: WoWObject(Guid)
{
	m_ObjectType |= TYPEMASK_ITEM;
	m_ObjectTypeId = TYPEID_ITEM;
	m_valuesCount = ITEM_END;
}

WoWItem::~WoWItem()
{
}

std::shared_ptr<WoWItem> WoWItem::Create(IBaseManager & BaseManager, IRenderDevice & RenderDevice, IScene * Scene, ObjectGuid Guid)
{
	std::shared_ptr<WoWItem> thisObj = std::make_shared<WoWItem>(Guid);
	return thisObj;
}

#endif