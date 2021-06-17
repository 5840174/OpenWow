#pragma once

#ifdef USE_WMO_MODELS

#include "WMO_Headers.h"

class WMO_Part_Fog
{
public:
	WMO_Part_Fog(const SWMO_FogDef& WMOFogProto);

	void setup();

public:
	glm::vec4 color;

private:
	SWMO_FogDef m_WMOFogProto;
};

#endif