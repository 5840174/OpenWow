#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWItem.h"

class ZN_API CWoWContainer
	: public WoWItem
{
public:
	CWoWContainer(CWoWGuid Guid);
	virtual ~CWoWContainer();

public:
	static std::shared_ptr<CWoWContainer> Create(IScene& Scene, CWoWGuid Guid);

private:

};

#endif