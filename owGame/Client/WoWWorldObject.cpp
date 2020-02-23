#include "stdafx.h"

// General
#include "WoWWorldObject.h"

WorldObject::WorldObject()
{
}

WorldObject::~WorldObject()
{
}

void WorldObject::UpdateMovementData(CByteBuffer & Bytes)
{
	UpdateMovementDataInternal(Bytes);
}



//
// protected:
//
void WorldObject::UpdateMovementDataInternal(CByteBuffer & Bytes)
{
	Bytes >> PositionX;
	Bytes >> PositionY;
	Bytes >> PositionZ;
	Bytes >> Orientation;

	glm::vec3 position = fromGameToReal(glm::vec3(PositionX, PositionY, PositionZ));
	SetTranslate(position);
	SetRotation(glm::vec3(0.0f, Orientation + glm::half_pi<float>(), 0.0f));
}
