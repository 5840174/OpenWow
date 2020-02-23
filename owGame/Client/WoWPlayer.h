#pragma once

#include "WoWUnit.h"

class ZN_API WoWPlayer
	: public WoWUnit
{
public:
	WoWPlayer();
	virtual ~WoWPlayer();

	void UpdateMovementData(CByteBuffer& Bytes) override final; // For inheritance

public:
	static std::shared_ptr<WoWPlayer> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid);

private:
	float transportX;
	float transportY;
	float transportZ;
	float transportO;
	uint64 transportGUID;
};