#pragma once

#include "DBC/DBC__File.h"


DBC_DEF_BEGIN(DBC_CharacterFacialHairStyles)

#if WOW_CLIENT_VERSION == WOW_CLASSIC_1_12_1
__DBC_TVALUE(uint32, Race, 1);
__DBC_TVALUE(uint32, Gender, 2);
__DBC_TVALUE(uint32, Variation, 3);
__DBC_TVALUE(uint32, Group_01xx, 4); // BeardGeoset
__DBC_TVALUE(uint32, Group_03xx, 5); // MoustacheGeoset
__DBC_TVALUE(uint32, Group_02xx, 6); // SideburnGeoset
__DBC_TVALUE(uint32, Other0, 7);
__DBC_TVALUE(uint32, Other1, 8);
__DBC_TVALUE(uint32, Other2, 9);
#else
__DBC_TVALUE(uint32, Race, 1);
__DBC_TVALUE(uint32, Gender, 2);
__DBC_TVALUE(uint32, Variation, 3);
__DBC_TVALUE(uint32, Group_01xx, 4); // BeardGeoset
__DBC_TVALUE(uint32, Group_03xx, 5); // MoustacheGeoset
__DBC_TVALUE(uint32, Group_02xx, 6); // SideburnGeoset
__DBC_TVALUE(uint32, Group_16xx, 6); // SideburnGeoset
__DBC_TVALUE(uint32, Group_17xx, 6); // SideburnGeoset
#endif

DBC_DEF_END
