#pragma once

#ifdef USE_WMO_MODELS

#include "WMO_Headers.h"

class WMO_Part_Light 
{
public:
	WMO_Part_Light(const SWMO_MOLT& WMOLightProto);

	glm::vec3               GetPosition() const { return Fix_From_XZmY_To_XYZ(m_WMOLightProto.pos); }
	const SWMO_MOLT&        GetProto() const { return m_WMOLightProto; }

private:
	const SWMO_MOLT m_WMOLightProto;
};

#endif