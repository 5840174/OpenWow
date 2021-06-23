#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_TransportAnimation)

__DBC_TVALUE(uint32, TransportID, 2);
__DBC_TVALUE(uint32, TimeIndex, 3);

__DBC_TVALUE(float, PositionX, 4);
__DBC_TVALUE(float, PositionY, 5);
__DBC_TVALUE(float, PositionZ, 6);

__DBC_TVALUE(uint32, SequenceID, 7);

DBC_DEF_END
