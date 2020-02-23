#pragma once

#include "WorldObject.h"

class ZN_API WoWGameObject
	: public WorldObject
{
public:
	WoWGameObject();
	virtual ~WoWGameObject();

public:
	static std::shared_ptr<WoWGameObject> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid);

private:

};