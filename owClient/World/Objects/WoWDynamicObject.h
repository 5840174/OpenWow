#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObject.h"

class ZN_API WoWDynamicObject
	: public WoWObject
{
public:
	WoWDynamicObject(CWoWObjectGuid Guid);
	virtual ~WoWDynamicObject();

public:
	static std::shared_ptr<WoWDynamicObject> Create(IScene& Scene, CWoWObjectGuid Guid);

private:

};

#endif
