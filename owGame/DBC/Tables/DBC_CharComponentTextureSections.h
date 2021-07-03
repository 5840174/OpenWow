#pragma once

#include "DBC/DBC__File.h"

enum class ZN_API DBC_CharComponent_Sections : uint32
{
	ARMS_UPPER = 0,
	ARMS_LOWER = 1,

	HANDS = 2,

	TORSO_UPPER = 3,
	TORSO_LOWER = 4,

	LEGS_UPPER = 5,
	LEGS_LOWER = 6,

	FEET = 7,

	ITEMS_COUNT = 8,

	SCALP_UPPER = 9,
	SCALP_LOWER = 10,

	COUNT = 11
};

DBC_DEF_BEGIN(DBC_CharComponentTextureSections)

__DBC_REF_ID(DBC_CharComponentTextureLayouts,		Layout,			2);
__DBC_TVALUE(DBC_CharComponent_Sections,		    Section,		3);
__DBC_TVALUE(uint32,								X,				4);
__DBC_TVALUE(uint32,								Y,				5);
__DBC_TVALUE(uint32,								Width,			6);
__DBC_TVALUE(uint32,								Height,			7);

DBC_DEF_END
