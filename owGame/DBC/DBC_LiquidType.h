#pragma once

#include "DBC/Core/DBC__File.h"

struct DBC_LIQUIDTYPE_Type
{
	enum List : uint32_t
	{
		water = 0,
		ocean,
		magma,
		slime
	};
};

DBC_DEF_BEGIN(DBC_LiquidType)

__DBC_STRING(Name, 2);
__DBC_TVALUE(uint32, Flags, 3);
__DBC_TVALUE(DBC_LIQUIDTYPE_Type::List, Type, 4);
__DBC_TVALUE(uint32, SpellID, 4);

DBC_DEF_END