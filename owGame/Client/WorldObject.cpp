#include "stdafx.h"

// General
#include "WorldObject.h"

WorldObject::WorldObject()
{
}

WorldObject::~WorldObject()
{
}

void WorldObject::UpdateMovementData(CByteBuffer & Bytes)
{
	Bytes >> PositionX;
	Bytes >> PositionY;
	Bytes >> PositionZ;
	Bytes >> Orientation;
}
