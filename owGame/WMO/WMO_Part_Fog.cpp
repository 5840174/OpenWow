#include "stdafx.h"

// General
#include "Wmo_Part_Fog.h"

WMO_Part_Fog::WMO_Part_Fog(const SWMO_FogDef& _proto)
{
	fogDef = _proto;

	color = vec4(fogDef.fog.color.r, fogDef.fog.color.g, fogDef.fog.color.b, fogDef.fog.color.a);
	fogDef.position = Fix_XZmY(fogDef.position);
	fogDef.fog.startScalar = fogDef.fog.startScalar * fogDef.fog.end;
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