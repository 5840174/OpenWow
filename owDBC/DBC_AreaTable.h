#pragma once

#include "DBC__File.h"

// FORWARD BEGIN
DBC_DEFINE(DB�_LiquidType);
DBC_DEFINE(DB�_Light);
// FORWARD END

DBC_DEF_BEGIN(DB�_AreaTable)

__DBC_REF_ID(DBC_Map,				ContinentID,			2);
__DBC_REF_ID(DB�_AreaTable,			ParentAreaNum,			3);

__DBC_TVALUE(uint32,				AreaBits,				4);
__DBC_TVALUE(uint32,				Flags,					5);

//__DBC_TVALUE(iRefID, SoundProviderPrefferences, 6);
//__DBC_TVALUE(iRefID, Underwater_SoundProviderPrefferences, 7);
//__DBC_TVALUE(iRefID, SoundAmbience, 8);
//__DBC_TVALUE(iRefID, ZoneMusic, 9);
//__DBC_TVALUE(iRefID, ZoneIntroMusicTable, 10);

__DBC_TVALUE(int32,					ExplorationLevel,		11);
__DBC_LOCSTR(						AreaName,				12);

__DBC_TVALUE(uint32,				FactionGroupMask,		20);
__DBC_REF_ID(DB�_LiquidType,		LiquidTypeID,			21);
__DBC_TVALUE(float,					MinElevation,			22);
__DBC_TVALUE(float,					AmbientMultiplier,		16);
__DBC_REF_ID(DB�_Light,				LightId,				17);

DBC_DEF_END