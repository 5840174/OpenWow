#include "stdafx.h"

// Includes

// General
#include "Wmo_Part_Light.h"

WMO_Part_Light::WMO_Part_Light(const SWMO_LightDef& WMOLightProto)
	: m_WMOLightProto(WMOLightProto)
{
	m_Position = vec3(m_WMOLightProto.pos.x, m_WMOLightProto.pos.z, -m_WMOLightProto.pos.y);

	fcolor = fromARGB(m_WMOLightProto.color);
	//fcolor *= m_WMOLightProto.intensity;
	fcolor.w = 1.0f;

	/*

	Light light;
	light.m_Type = Light::LightType::Point;
	light.m_PositionWS = vec4(m_WMOLightProto.pos.x, m_WMOLightProto.pos.z, -m_WMOLightProto.pos.y, 1.0f);
	light.m_Color = fcolor;
	light.m_Range = _proto.intensity;

	setLight(light);
	*/

	Log::Warn("WMOLight: Created: Type %d, UseAtten %d, %f", m_WMOLightProto.type, m_WMOLightProto.useAtten, m_WMOLightProto.attenStart, m_WMOLightProto.attenEnd);
}

void WMO_Part_Light::setup(uint32 light)
{
	float LightAmbient[] = {0, 0, 0, 1.0f};
	float LightPosition[] = { m_Position.x, m_Position.y, m_Position.z, 0.0f};

	//glLightfv(light, GL_AMBIENT, LightAmbient);
	//glLightfv(light, GL_DIFFUSE, fcolor);
	//glLightfv(light, GL_POSITION, LightPosition);

	//glEnable(light);
}
