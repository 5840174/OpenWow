#pragma once

#include "DBC__File.h"

DBC_DEF_BEGIN(DBC_GroundEffectTexture)

__DBC_TARRAY(uint32,	DoodadId, 2, 4);
__DBC_TVALUE(uint32,	Density, 6);
__DBC_TVALUE(uint32,	Sound, 7);

DBC_DEF_END