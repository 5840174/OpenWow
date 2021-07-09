#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "Base/WoWObject.h"

class ZN_API WoWDynamicObject
	: public WoWObject
{
public:
	WoWDynamicObject(CWoWGuid Guid);
	virtual ~WoWDynamicObject();

public:
	static std::shared_ptr<WoWDynamicObject> Create(IScene& Scene, CWoWGuid Guid);

private:

};

#endif
