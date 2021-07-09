#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "../Base/WoWObject.h"

class ZN_API WoWItem
	: public WoWObject
{
public:
	WoWItem(CWoWGuid Guid);
	virtual ~WoWItem();

public:
	static std::shared_ptr<WoWItem> Create(IScene& Scene, CWoWGuid Guid);

private:

};

#endif
