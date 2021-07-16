#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "Base/WoWObject.h"

class ZN_API CowServerDynamicObject
	: public CowServerObject
{
public:
	CowServerDynamicObject(CowGuid Guid);
	virtual ~CowServerDynamicObject();

public:
	static std::shared_ptr<CowServerDynamicObject> Create(IScene& Scene, CowGuid Guid);

private:

};

#endif
