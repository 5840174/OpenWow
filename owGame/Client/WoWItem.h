#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObject.h"

class ZN_API WoWItem
	: public WoWObject
{
public:
	WoWItem(ObjectGuid Guid);
	virtual ~WoWItem();

public:
	static std::shared_ptr<WoWItem> Create(IScene& Scene, ObjectGuid Guid);

private:

};

#endif
