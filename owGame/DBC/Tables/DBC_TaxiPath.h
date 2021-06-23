#pragma once

#include "DBC/DBC__File.h"

DBC_DEF_BEGIN(DBC_TaxiPath)

__DBC_TVALUE(uint32, FromTaxiNodeID, 2);
__DBC_TVALUE(uint32, ToTaxiNodeID,   3);
__DBC_TVALUE(uint32, CostInCooper,   4);

DBC_DEF_END
