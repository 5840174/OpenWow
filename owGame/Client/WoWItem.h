#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObject.h"

class ZN_API WoWItem
	: public WoWObject
{
public:
	WoWItem(CWoWObjectGuid Guid);
	virtual ~WoWItem();

public:
	static std::shared_ptr<WoWItem> Create(IScene& Scene, CWoWObjectGuid Guid);

private:

};

#endif
