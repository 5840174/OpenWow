#pragma once

#include "Object.h"
#include "WorldLocation.h"

class ZN_API WorldObject
	: public WoWObject
	, public WorldLocation
{
public:
	WorldObject();
	virtual ~WorldObject();

	virtual void UpdateMovementData(CByteBuffer& Bytes) override; // For inheritance

private:
	std::string m_name;
};