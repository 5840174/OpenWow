#pragma once

#include "WMO_Headers.h"

class WMO_Part_Fog
{
public:
	WMO_Part_Fog(const SWMO_FogDef& WMOFogProto);

	void setup();

public:
	vec4 color;

private:
	SWMO_FogDef m_WMOFogProto;
};