#include "stdafx.h"

// General
#include "SkyParams.h"

CSkyParams::CSkyParams()
{
	Clear();
}

CSkyParams::~CSkyParams()
{}


const CSkyParams& CSkyParams::operator+=(const CSkyParams& Other)
{
	for (uint8 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
		m_Colors[i] += Other.m_Colors[i];

	for (uint8 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
		m_Fogs[i] += Other.m_Fogs[i];

	m_glow += Other.m_glow;
	m_oceanShallowAlpha += Other.m_oceanShallowAlpha;
	m_oceanDeepAlpha += Other.m_oceanDeepAlpha;
	m_waterShallowAlpha += Other.m_waterShallowAlpha;
	m_waterDeepAlpha += Other.m_waterDeepAlpha;

	return *this;
}

const CSkyParams& CSkyParams::operator*=(float Weight)
{
	for (uint8 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
		m_Colors[i] *= Weight;

	for (uint8 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
		m_Fogs[i] *= Weight;

	m_glow *= Weight;
	m_oceanShallowAlpha *= Weight;
	m_oceanDeepAlpha *= Weight;
	m_waterShallowAlpha *= Weight;
	m_waterDeepAlpha *= Weight;

	return *this;
}

void CSkyParams::Clear()
{
	for (uint8 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
		m_Colors[i] = glm::vec3(0.0f, 0.0f, 0.0f);

	for (uint8 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
		m_Fogs[i] = 0.0f;

	m_glow = 0.0f;
	m_waterShallowAlpha = 0.0f;
	m_waterDeepAlpha = 0.0f;
	m_oceanShallowAlpha = 0.0f;
	m_oceanDeepAlpha = 0.0f;
}

void CSkyParams::SetColor(uint32 ColorType, glm::vec3 Value)
{
	_ASSERT(ColorType >= 0 && ColorType < ESkyColors::SKY_COLOR_COUNT);
	m_Colors[ColorType] = Value;
}

glm::vec3 CSkyParams::GetColor(uint32 ColorType) const
{
	_ASSERT(ColorType >= 0 && ColorType < ESkyColors::SKY_COLOR_COUNT);
	return m_Colors[ColorType];
}

void CSkyParams::SetFog(uint32 FogType, float Value)
{
	_ASSERT(FogType >= 0 && FogType < ESkyFogs::SKY_FOG_COUNT);
	m_Fogs[FogType] = Value;
}

float CSkyParams::GetFog(uint32 FogType) const
{
	_ASSERT(FogType >= 0 && FogType < ESkyFogs::SKY_FOG_COUNT);
	return m_Fogs[FogType];
}

void CSkyParams::SetHighlightSky(bool Value)
{
	m_highlightSky = Value;
}

bool CSkyParams::GetHighlightSky() const
{
	return m_highlightSky;
}

void CSkyParams::SetSkybox(const DBC_LightSkyboxRecord* Value)
{
	m_SkyBox = Value;
}

const DBC_LightSkyboxRecord* CSkyParams::GetSkybox() const
{
	return m_SkyBox;
}

void CSkyParams::SetGlow(float GlowValue)
{
	m_glow = GlowValue;
}

float CSkyParams::GetGlow() const
{
	return m_glow;
}

void CSkyParams::SetWaterAplha(uint32 WaterAlphaType, float Value)
{
	_ASSERT(WaterAlphaType >= 0 && WaterAlphaType < ESkyWaterAlpha::SKY_COUNT);
	m_WaterAplha[WaterAlphaType] = Value;
}

float CSkyParams::GetWaterAplha(uint32 WaterAlphaType) const
{
	_ASSERT(WaterAlphaType >= 0 && WaterAlphaType < ESkyWaterAlpha::SKY_COUNT);
	return m_WaterAplha[WaterAlphaType];
}
