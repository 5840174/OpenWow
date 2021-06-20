#pragma once

#include "DBC/DBC__File.h"

enum class DBC_LIQUIDTYPE_Type : uint32
{
	water = 0,
	ocean,
	magma,
	slime
};

#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5

DBC_DEF_BEGIN(DBC_LiquidType)

__DBC_STRING(Name, 2);
__DBC_TVALUE(uint32, Flags, 3);
__DBC_TVALUE(DBC_LIQUIDTYPE_Type::List, Type, 4);
__DBC_TVALUE(uint32, SpellID, 4);

DBC_DEF_END

#else

DBC_DEF_BEGIN(DBC_LiquidType)

__DBC_STRING(Name, 2);
__DBC_TVALUE(uint32, Flags, 3);
__DBC_TVALUE(DBC_LIQUIDTYPE_Type, Type, 4);
__DBC_TVALUE(uint32, SoundEntriesID, 5);
__DBC_TVALUE(uint32, SpellID, 6);

__DBC_TVALUE(float, maxDarkenDepth, 7);
__DBC_TVALUE(float, fogDarkenIntensity, 8);
__DBC_TVALUE(float, ambDarkenIntensity, 9);
__DBC_TVALUE(float, dirDarkenIntensity, 10);

__DBC_TVALUE(uint32, LightID, 11);

__DBC_TVALUE(float, particleScale, 12);
__DBC_TVALUE(uint32, particleMovement, 13);
__DBC_TVALUE(uint32, particleTexSlots, 14);

//__DBC_REF_ID(DBC_LiquidMaterial, LiquidMaterialID, 15)

__DBC_STRARR(Texture, 16, 6);
__DBC_TARRAY(uint32, color, 22, 2);
__DBC_TARRAY(uint32, floats, 24, 18);
__DBC_TARRAY(uint32, ints, 42, 4);

DBC_DEF_END

#endif