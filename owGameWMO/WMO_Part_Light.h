#pragma once

enum LightType
{
	OMNI_LGT,
	SPOT_LGT,
	DIRECT_LGT,
	AMBIENT_LGT
};

#include "WMO_Headers.h"

class WMO_Part_Light : public CRefItem
{
public:
	WMO_Part_Light(const SWMO_LightDef& _proto);

	void setup(uint32 light);

public:
	vec4 fcolor;

	SWMO_LightDef lightDef;
};