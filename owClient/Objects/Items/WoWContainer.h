#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWItem.h"

class ZN_API CowServerContainer
	: public CowServerItem
{
public:
	CowServerContainer(CowGuid Guid);
	virtual ~CowServerContainer();

public:
	static std::shared_ptr<CowServerContainer> Create(IScene& Scene, CowGuid Guid);

private:

};

#endif