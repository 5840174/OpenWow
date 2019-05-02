#include "stdafx.h"

// General
#include "SkyParams.h"

CSkyParams::CSkyParams()
{
	Clear();
}

CSkyParams::~CSkyParams()
{}


const CSkyParams& CSkyParams::operator+=(const CSkyParams& _s)
{
    for (uint8 i = 0; i < LightColors::COUNT; i++)
        m_Colors[i] += _s.m_Colors[i];

    for (uint8 i = 0; i < LightFogs::COUNT; i++)
        m_Fogs[i] += _s.m_Fogs[i];

    m_glow += _s.m_glow;
    m_oceanShallowAlpha += _s.m_oceanShallowAlpha;
    m_oceanDeepAlpha += _s.m_oceanDeepAlpha;
    m_waterShallowAlpha += _s.m_waterShallowAlpha;
    m_waterDeepAlpha += _s.m_waterDeepAlpha;

    return *this;
}

const CSkyParams& CSkyParams::operator*=(float _weight)
{
    for (uint8 i = 0; i < LightColors::COUNT; i++)
        m_Colors[i] *= _weight;

    for (uint8 i = 0; i < LightFogs::COUNT; i++)
        m_Fogs[i] *= _weight;

    m_glow *= _weight;
    m_oceanShallowAlpha *= _weight;
    m_oceanDeepAlpha *= _weight;
    m_waterShallowAlpha *= _weight;
    m_waterDeepAlpha *= _weight;

    return *this;
}

void CSkyParams::Clear()
{
	for (uint8 i = 0; i < LightColors::COUNT; i++)
		m_Colors[i] = vec3(1.0f, 1.0f, 1.0f);
	for (uint8 i = 0; i < LightFogs::COUNT; i++)
		m_Fogs[i] = 0.0f;

	m_glow = 0.0f;
	m_waterShallowAlpha = 0.0f;
	m_waterDeepAlpha = 0.0f;
	m_oceanShallowAlpha = 0.0f;
	m_oceanDeepAlpha = 0.0f;
}

void CSkyParams::SetColor(uint32 ColorType, vec3 Value)
{
    _ASSERT(ColorType >= 0 && ColorType < LightColors::COUNT);
    m_Colors[ColorType] = Value;
}

vec3 CSkyParams::GetColor(uint32 ColorType) const
{
    _ASSERT(ColorType >= 0 && ColorType < LightColors::COUNT);
    return m_Colors[ColorType];
}

void CSkyParams::SetFog(uint32 FogType, float Value)
{
    _ASSERT(FogType >= 0 && FogType < LightFogs::COUNT);
    m_Fogs[FogType] = Value;
}

float CSkyParams::GetFog(uint32 FogType) const
{
    _ASSERT(FogType >= 0 && FogType < LightFogs::COUNT);
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

void CSkyParams::SetSkybox(std::shared_ptr<DBC_LightSkyboxRecord> Value)
{
    m_SkyBox = Value;
}

std::shared_ptr<DBC_LightSkyboxRecord> CSkyParams::GetSkybox() const
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
    _ASSERT(WaterAlphaType >= 0 && WaterAlphaType < LightWaterAlpha::COUNT);
    m_WaterAplha[WaterAlphaType] = Value;
}

float CSkyParams::GetWaterAplha(uint32 WaterAlphaType) const
{
    _ASSERT(WaterAlphaType >= 0 && WaterAlphaType < LightWaterAlpha::COUNT);
    return m_WaterAplha[WaterAlphaType];
}
