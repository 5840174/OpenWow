#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_TaxiNodes)

__DBC_TVALUE(uint32, MapID,                    2);

__DBC_TVALUE(float,  X,                        3);
__DBC_TVALUE(float,  Y,                        4);
__DBC_TVALUE(float,  Z,                        5);

__DBC_LOCSTR(        Name,                     6);

__DBC_TVALUE(uint32, MountCreatureAllianceID,  23);
__DBC_TVALUE(uint32, MountCreatureHordeID,     24);

DBC_DEF_END
