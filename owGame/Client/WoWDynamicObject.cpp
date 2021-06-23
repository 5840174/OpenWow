#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWDynamicObject.h"

WoWDynamicObject::WoWDynamicObject(CWoWObjectGuid Guid)
	: WoWObject(Guid)
{
	m_ObjectType |= TYPEMASK_DYNAMICOBJECT;
	m_ObjectTypeId = TYPEID_DYNAMICOBJECT;
	m_valuesCount = DYNAMICOBJECT_END;
}

WoWDynamicObject::~WoWDynamicObject()
{
}

std::shared_ptr<WoWDynamicObject> WoWDynamicObject::Create(IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWDynamicObject> thisObj = std::make_shared<WoWDynamicObject>(Guid);
	return thisObj;
}

#endif