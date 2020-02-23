#pragma once

#include "WoWObject.h"

class ZN_API WoWItem
	: public WoWObject
{
public:
	WoWItem();
	virtual ~WoWItem();

public:
	static std::shared_ptr<WoWItem> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid);

private:

};
