#pragma once

#include "WMO_Headers.h"

class WMO_Part_Fog
{
public:
	WMO_Part_Fog(const SWMO_FogDef& _proto);

	void setup();

public:
	vec4 color;

	SWMO_FogDef fogDef;
};