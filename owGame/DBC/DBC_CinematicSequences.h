#pragma once

#include "DBC/Core/DBC__File.h"

DBC_DEF_BEGIN(DBC_CinematicSequences)

__DBC_TVALUE(uint32,				SoundID,		2);
__DBC_REF_ID(DBC_CinematicCamera,	CameraRec,		3);

DBC_DEF_END