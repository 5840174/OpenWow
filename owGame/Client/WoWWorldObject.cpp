#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWWorldObject.h"

WorldObject::WorldObject(IScene& Scene, CWoWObjectGuid Guid)
	: WoWObject(Guid)
	, WorldLocation(Scene)
	, DestinationPoint(0.0f)
{
}

WorldObject::~WorldObject()
{
}

#endif
