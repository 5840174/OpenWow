#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWDynamicObject.h"

CowServerDynamicObject::CowServerDynamicObject(CowGuid Guid)
	: CowServerObject(Guid)
{
	//m_ObjectType |= TYPEMASK_DYNAMICOBJECT;
	m_Values.SetValuesCount(DYNAMICOBJECT_END);
}

CowServerDynamicObject::~CowServerDynamicObject()
{
}

std::shared_ptr<CowServerDynamicObject> CowServerDynamicObject::Create(IScene& Scene, CowGuid Guid)
{
	std::shared_ptr<CowServerDynamicObject> thisObj = MakeShared(CowServerDynamicObject, Guid);
	return thisObj;
}

#endif