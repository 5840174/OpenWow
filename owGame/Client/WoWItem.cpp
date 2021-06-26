#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWItem.h"

WoWItem::WoWItem(CWoWObjectGuid Guid)
	: WoWObject(Guid)
{
	m_ObjectType |= TYPEMASK_ITEM;
	m_Values.SetValuesCount(ITEM_END);
}

WoWItem::~WoWItem()
{
}

std::shared_ptr<WoWItem> WoWItem::Create(IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWItem> thisObj = MakeShared(WoWItem, Guid);
	return thisObj;
}

#endif