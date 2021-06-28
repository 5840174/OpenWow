#pragma once

#ifdef USE_WMO_MODELS

#include "WMO_Headers.h"

class WMO_Part_Fog
{
public:
	WMO_Part_Fog(const SWMO_MFOG& WMOFogProto);

	void setup();

public:
	glm::vec4 color;

private:
	SWMO_MFOG m_WMOFogProto;
};

#endif