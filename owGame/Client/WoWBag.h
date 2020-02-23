#pragma once

#include "WoWItem.h"

class ZN_API WoWBag
	: public WoWItem
{
public:
	WoWBag();
	virtual ~WoWBag();

public:
	static std::shared_ptr<WoWBag> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid);

private:

};