#pragma once

#include "WoWObject.h"
#include "WorldLocation.h"

class ZN_API WorldObject
	: public WoWObject
	, public WorldLocation
{
public:
	WorldObject(ObjectGuid Guid);
	virtual ~WorldObject();

	//virtual void UpdateMovementData(CByteBuffer& Bytes) override; // For inheritance

protected:
	void UpdateMovementDataInternal(CByteBuffer& Bytes);

private:
	std::string m_name;
};