#pragma once

#include "DBC/DBC__File.h"

enum class DBC_LIQUIDTYPE_Type : uint32
{
	water = 0,
	ocean,
	magma,
	slime
};

DBC_DEF_BEGIN(DBC_LiquidType)

__DBC_STRING(Name, 2);
__DBC_TVALUE(uint32, Flags, 3);
__DBC_TVALUE(DBC_LIQUIDTYPE_Type, Type, 4);
__DBC_TVALUE(uint32, SpellID, 4);

DBC_DEF_END
