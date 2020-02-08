#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_CreatureDisplayInfo)

__DBC_REF_ID(DBC_CreatureModelData,                 Model,              2);
__DBC_TVALUE(uint32,                                SoundID,            3);
__DBC_REF_ID(DBC_CreatureDisplayInfoExtra,          HumanoidData,       4);
__DBC_TVALUE(float_t,                               Scale,              5);
__DBC_TVALUE(uint32,                                Opacity,            6); // 0 - 255

__DBC_STRING(Texture1,                                                  7); // Name of texture for 1st geoset with type 2 (see this). Texture must be in the same dir as M2 file of creature is.
__DBC_STRING(Texture2,                                                  8); // Name of texture for 2nd geoset with type 2 (see this). Texture must be in the same dir as M2 file of creature is.
__DBC_STRING(Texture3,                                                  9); // Name of texture for 3rd geoset with type 2 (see this). Texture must be in the same dir as M2 file of creature is.

__DBC_TVALUE(uint32,                                SizeClass,          10);
__DBC_TVALUE(uint32,                                BloodID,            11);
__DBC_TVALUE(uint32,                                NPCSoundID,         12);

DBC_DEF_END
