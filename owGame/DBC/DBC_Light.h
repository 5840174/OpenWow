#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_Light)

__DBC_REF_ID(DBC_Map,			MapID,				2);
__DBC_TVALUE(float,				PositionX,			3);
__DBC_TVALUE(float,				PositionY,			4);
__DBC_TVALUE(float,				PositionZ,			5);
__DBC_TVALUE(float,				RadiusInner,		6);
__DBC_TVALUE(float,				RadiusOuter,		7);
__DBC_TARRAY(uint32,			LightParams,		8, 8);


//__DBC_REF_ID(DBC_LightParams,	LightParamsID,		8);

DBC_DEF_END