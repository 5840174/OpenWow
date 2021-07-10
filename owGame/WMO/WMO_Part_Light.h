#pragma once

#ifdef USE_WMO_MODELS

enum LightType
{
	OMNI_LGT,
	SPOT_LGT,
	DIRECT_LGT,
	AMBIENT_LGT
};

#include "WMO_Headers.h"

class WMO_Part_Light 
{
public:
	WMO_Part_Light(const SWMO_MOLT& WMOLightProto);

	void setup(uint32 light);

public:
	glm::vec3 m_Position;
	ColorRGBA fcolor;

private:
	const SWMO_MOLT m_WMOLightProto;
};

#endif