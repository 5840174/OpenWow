#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "../Base/WoWObject.h"

class ZN_API CowServerItem
	: public CowServerObject
{
public:
	CowServerItem(CowGuid Guid);
	virtual ~CowServerItem();

public:
	static std::shared_ptr<CowServerItem> Create(IScene& Scene, CowGuid Guid);

private:

};

#endif
