#pragma once

#include "DBC/DBC__File.h"

enum class DBC_GameObjectDisplayInfo_SoundSlot : uint32
{
	stand = 0,
	open,
	loop,
	close,
	destroy,
	opened,
	custom0,
	custom1,
	custom2,
	custom3,
	SOUND_SLOT_COUNT, // = 10
};

DBC_DEF_BEGIN(DBC_GameObjectDisplayInfo)

__DBC_STRING(        ModelName,               2);
__DBC_TARRAY(uint32, SoundEntryID,            3, DBC_GameObjectDisplayInfo_SoundSlot::SOUND_SLOT_COUNT)

__DBC_TVALUE(float,  BBoxMinX,               13)
__DBC_TVALUE(float,  BBoxMinY,               14)
__DBC_TVALUE(float,  BBoxMinZ,               15)

__DBC_TVALUE(float,  BBoxMaxX,               16)
__DBC_TVALUE(float,  BBoxMaxY,               17)
__DBC_TVALUE(float,  BBoxMaxZ,               18)

__DBC_TVALUE(uint32, ObjectEffectPackageID,  19);
DBC_DEF_END