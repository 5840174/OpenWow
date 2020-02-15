#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_CinematicCamera)

__DBC_STRING(			Filename,	2);
__DBC_TVALUE(uint32,	VoiceID,	3);

__DBC_TVALUE(float,		EndX,		4);
__DBC_TVALUE(float,		EndY,		5);
__DBC_TVALUE(float,		EndZ,		6);

__DBC_TVALUE(float,		Rotation,	7);

DBC_DEF_END