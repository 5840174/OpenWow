#pragma once

#include "WoWObject.h"

class ZN_API WoWDynamicObject
	: public WoWObject
{
public:
	WoWDynamicObject(ObjectGuid Guid);
	virtual ~WoWDynamicObject();

public:
	static std::shared_ptr<WoWDynamicObject> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid);

private:

};
