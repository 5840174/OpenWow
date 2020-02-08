#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_CharHairGeosets)

__DBC_TVALUE(uint32, Race, 2);
__DBC_TVALUE(uint32, Gender, 3);
__DBC_TVALUE(uint32, HairType, 4);
__DBC_TVALUE(uint32, Geoset, 5);
__DBC_TVALUE(uint32, Bald, 6);

DBC_DEF_END
