#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_CharacterFacialHairStyles)

__DBC_TVALUE(uint32, Race, 1);
__DBC_TVALUE(uint32, Gender, 2);
__DBC_TVALUE(uint32, Variation, 3);
__DBC_TVALUE(uint32, Group_01xx, 4);
__DBC_TVALUE(uint32, Group_03xx, 5);
__DBC_TVALUE(uint32, Group_02xx, 6);
__DBC_TVALUE(uint32, Group_16xx, 7);
__DBC_TVALUE(uint32, Group_17xx, 8);

DBC_DEF_END
