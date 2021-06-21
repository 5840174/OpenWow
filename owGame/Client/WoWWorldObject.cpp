#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWWorldObject.h"

WorldObject::WorldObject(IScene& Scene, ObjectGuid Guid)
	: WoWObject(Guid)
	, WorldLocation(Scene)
{
}

WorldObject::~WorldObject()
{
}

#endif
