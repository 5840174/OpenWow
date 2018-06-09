#include "stdafx.h"

// General
#include "Wmo_Fog.h"

WMOFog::WMOFog(IFile* f)
{
	f->ReadBytes(&fogDef, sizeof(WMO_FogDef));
	color = vec4(fogDef.fog.color.r, fogDef.fog.color.g, fogDef.fog.color.b, fogDef.fog.color.a);
	fogDef.position.toXZmY();
	fogDef.fog.startScalar = fogDef.fog.startScalar * fogDef.fog.end;
}

void WMOFog::setup()
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