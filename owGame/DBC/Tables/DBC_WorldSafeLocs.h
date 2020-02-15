#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_WorldSafeLocs)

__DBC_REF_ID(DBC_Map, MapID, 2);
__DBC_TVALUE(float, X, 3);
__DBC_TVALUE(float, Y, 4);
__DBC_TVALUE(float, Z, 5);
__DBC_LOCSTR(Name, 6);

DBC_DEF_END