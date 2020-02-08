#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_Material)

__DBC_TVALUE(uint32, Flags, 2);

__DBC_TVALUE(uint32, FoleySoundID, 3);
__DBC_TVALUE(uint32, SheatheSoundID, 4);
__DBC_TVALUE(uint32, UnSheatheSoundID, 5);

DBC_DEF_END
