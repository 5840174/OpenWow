#pragma once

#if 0

#include "WoWObject.h"

class ZN_API WoWItem
	: public WoWObject
{
public:
	WoWItem(ObjectGuid Guid);
	virtual ~WoWItem();

public:
	static std::shared_ptr<WoWItem> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid);

private:

};

#endif
