#pragma once

#if 0

#include "WoWObject.h"
#include "WorldLocation.h"

class ZN_API WorldObject
	: public WoWObject
	, public WorldLocation
{
public:
	WorldObject(ObjectGuid Guid);
	virtual ~WorldObject();

private:
	std::string m_name;
};

#endif