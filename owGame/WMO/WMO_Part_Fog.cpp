#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "Wmo_Part_Fog.h"

WMO_Part_Fog::WMO_Part_Fog(const SWMO_FogDef& WMOFogProto)
	: m_WMOFogProto(WMOFogProto)
{
	color = glm::vec4(m_WMOFogProto.fog.color.r, m_WMOFogProto.fog.color.g, m_WMOFogProto.fog.color.b, m_WMOFogProto.fog.color.a);
	m_WMOFogProto.position = Fix_XZmY(m_WMOFogProto.position);
	m_WMOFogProto.fog.startScalar = m_WMOFogProto.fog.startScalar * m_WMOFogProto.fog.end;
}

void WMO_Part_Fog::setup()
{
	/*if (_Config.drawfog)
	{
		glFogfv(GL_FOG_COLOR, color);
		glFogf(GL_FOG_START, fogDef.fog.startScalar);
		glFogf(GL_FOG_END, fogDef.fog.end);

		glEnable(GL_FOG);
	}
	else
	{
		glDisable(GL_FOG);
	}*/
}

#endif