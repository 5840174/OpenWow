#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObject.h"
#include "WorldLocation.h"

class ZN_API WorldObject
	: public WoWObject
	, public WorldLocation
{
public:
	WorldObject(IScene& Scene, ObjectGuid Guid);
	virtual ~WorldObject();

private:
	std::string m_name;
};

#endif