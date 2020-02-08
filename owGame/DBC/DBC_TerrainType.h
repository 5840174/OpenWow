#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_TerrainType)

__DBC_STRING(Desc, 2);
__DBC_TVALUE(uint32, FootstepSprayRun, 3);
__DBC_TVALUE(uint32, FootstepSprayWalk, 4);
__DBC_TVALUE(uint32, SoundID, 5);
__DBC_TVALUE(uint32, Flags, 6);

DBC_DEF_END
