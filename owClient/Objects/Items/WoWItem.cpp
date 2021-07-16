#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWItem.h"

CowServerItem::CowServerItem(CowGuid Guid)
	: CowServerObject(Guid)
{
	//m_ObjectType |= TYPEMASK_ITEM;
	m_Values.SetValuesCount(ITEM_END);
}

CowServerItem::~CowServerItem()
{
}

std::shared_ptr<CowServerItem> CowServerItem::Create(IScene& Scene, CowGuid Guid)
{
	std::shared_ptr<CowServerItem> thisObj = MakeShared(CowServerItem, Guid);
	return thisObj;
}

#endif