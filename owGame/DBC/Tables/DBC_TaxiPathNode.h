#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_TaxiPathNode)

__DBC_TVALUE(uint32, TaxiPathID, 2); // (Ref to TaxiPath.dbc) 
__DBC_TVALUE(uint32, NodeIndex,  3);
__DBC_TVALUE(uint32, MapID,      4);

__DBC_TVALUE(float,  X,          5);
__DBC_TVALUE(float,  Y,          6);
__DBC_TVALUE(float,  Z,          7);

__DBC_TVALUE(uint32, Flags,      8); // 2 is used for stopping the object for a number of seconds defined in column 9.
__DBC_TVALUE(uint32, Delay,      9);

__DBC_TVALUE(uint32, ArrivalEventID,   10);
__DBC_TVALUE(uint32, DepartureEventID, 11);

DBC_DEF_END
