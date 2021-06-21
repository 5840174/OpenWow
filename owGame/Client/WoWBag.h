#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWItem.h"

class ZN_API WoWBag
	: public WoWItem
{
public:
	WoWBag(ObjectGuid Guid);
	virtual ~WoWBag();

public:
	static std::shared_ptr<WoWBag> Create(IScene& Scene, ObjectGuid Guid);

private:

};

#endif