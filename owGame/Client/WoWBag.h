#pragma once

#include "WoWItem.h"

class ZN_API WoWBag
	: public WoWItem
{
public:
	WoWBag(ObjectGuid Guid);
	virtual ~WoWBag();

public:
	static std::shared_ptr<WoWBag> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid);

private:

};