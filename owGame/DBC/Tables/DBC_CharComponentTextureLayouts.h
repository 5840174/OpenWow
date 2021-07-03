#pragma once

#include "DBC/DBC__File.h"

enum class DBC_CharComponentTextureLayouts_Layout : uint32
{
	LAYOUT_1 = 1,
	LAYOUT_2 = 2
};

DBC_DEF_BEGIN(DBC_CharComponentTextureLayouts)

__DBC_TVALUE(uint32, Width, 2);
__DBC_TVALUE(uint32, Height, 3);

DBC_DEF_END
