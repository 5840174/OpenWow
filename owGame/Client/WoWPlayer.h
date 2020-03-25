#pragma once

#include "WoWUnit.h"

class ZN_API WoWPlayer
	: public WoWUnit
{
public:
	WoWPlayer(ObjectGuid Guid);
	virtual ~WoWPlayer();

public:
	static std::shared_ptr<WoWPlayer> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid);

private:
	float transportX;
	float transportY;
	float transportZ;
	float transportO;
	uint64 transportGUID;
};