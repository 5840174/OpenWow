#include "stdafx.h"

// Include
#include "SkyManager.h"

// General
#include "Sky.h"

const float cSkyMultiplier = 36.0f;

Sky::Sky() :
	m_LightRecord(nullptr)
{}

Sky::Sky(const CDBCStorage* DBCStorage, const std::shared_ptr<DBC_LightRecord>& LightData)
	: m_LightRecord(LightData)
{
	m_Position.x = m_LightRecord->Get_PositionX() / cSkyMultiplier;
	m_Position.y = m_LightRecord->Get_PositionY() / cSkyMultiplier;
	m_Position.z = m_LightRecord->Get_PositionZ() / cSkyMultiplier;
	m_Range.min = m_LightRecord->Get_RadiusInner() / cSkyMultiplier;
	m_Range.max = m_LightRecord->Get_RadiusOuter() / cSkyMultiplier;

	m_IsGlobalSky = (m_Position.x == 0.0f && m_Position.y == 0.0f && m_Position.z == 0.0f);
	if (m_IsGlobalSky)
		Log::Info("Sky: [%d] is global sky.", m_LightRecord->Get_ID());

	LoadParams(DBCStorage, LightParamsNames::ParamsClear);
}

Sky::~Sky()
{}


class SkyParam_Color : public Sky::SkyParam<vec3>
{
public:
	SkyParam_Color(uint32 _time, uint32 _color)
		: SkyParam<vec3>(_time, fromRGB(_color))
	{}
};


class SkyParam_Fog : public Sky::SkyParam<float>
{
public:
	SkyParam_Fog(uint32 _time, float _param)
		: SkyParam<float>(_time, _param)
	{}
};

void Sky::LoadParams(const CDBCStorage* DBCStorage, LightParamsNames _param)
{
	for (uint32 i = 0; i < LightColors::COUNT; i++)
	{
		m_IntBand_Colors[i].clear();
	}
	for (uint32 i = 0; i < LightFogs::COUNT; i++)
	{
		m_FloatBand_Fogs[i].clear();
	}

	std::shared_ptr<const DBC_LightParamsRecord> paramRecord = DBCStorage->DBC_LightParams()[m_LightRecord->Get_LightParams(static_cast<uint8>(_param))];
	_ASSERT(paramRecord != nullptr);
	uint32 paramSet = paramRecord->Get_ID();

	//-- LightParams
	m_Params.SetHighlightSky(paramRecord->Get_HighlightSky());
	m_Params.SetSkybox(DBCStorage->DBC_LightSkybox()[paramRecord->Get_LightSkyboxID()]);
	m_Params.SetGlow(paramRecord->Get_Glow());
	m_Params.SetWaterAplha(LightWaterAlpha::WATER_SHALLOW, paramRecord->Get_WaterShallowAlpha());
	m_Params.SetWaterAplha(LightWaterAlpha::WATER_DEEP, paramRecord->Get_WaterDeepAlpha());
	m_Params.SetWaterAplha(LightWaterAlpha::OCEAN_SHALLOW, paramRecord->Get_OceanShallowAlpha());
	m_Params.SetWaterAplha(LightWaterAlpha::OCEAN_DEEP, paramRecord->Get_OceanDeepAlpha());

	if (m_Params.GetSkybox() != nullptr)
	{
		Log::Info("Sky: Skybox name = '%s'.", m_Params.GetSkybox()->Get_Filename());
	}

	//-- Color params
	for (uint32 i = 0; i < LightColors::COUNT; i++)
	{
		std::shared_ptr<const DBC_LightIntBandRecord> lightColorsRecord = DBCStorage->DBC_LightIntBand()[paramSet * LightColors::COUNT - (LightColors::COUNT - 1) + i];
		_ASSERT(lightColorsRecord != nullptr);
		for (uint32 l = 0; l < lightColorsRecord->Get_Count(); l++)
		{
			// Read time & color value
			m_IntBand_Colors[i].push_back(SkyParam_Color(lightColorsRecord->Get_Times(l), lightColorsRecord->Get_Values(l)));
		}
	}

	//-- Fog, Sun, Clouds param
	for (uint32 i = 0; i < LightFogs::COUNT; i++)
	{
		std::shared_ptr<DBC_LightFloatBandRecord> lightFogRecord = DBCStorage->DBC_LightFloatBand()[paramSet * LightFogs::COUNT - (LightFogs::COUNT - 1) + i];
		_ASSERT(lightFogRecord != nullptr);
		for (uint32 l = 0; l < lightFogRecord->Get_Count(); l++)
		{
			// Read time & fog param
			float param = lightFogRecord->Get_Values(l);
			if (i == LightFogs::LIGHT_FOG_DISTANCE)	param /= cSkyMultiplier;
			m_FloatBand_Fogs[i].push_back(SkyParam_Fog(lightFogRecord->Get_Times(l), param));
		}
	}
}

CSkyParams& Sky::Interpolate(uint32 _time)
{
	for (uint8 i = 0; i < LightColors::COUNT; i++)
		m_Params.SetColor(i, GetByTimeTemplate(m_IntBand_Colors, i, _time));

	for (uint8 i = 0; i < LightFogs::COUNT; i++)
		m_Params.SetFog(i, GetByTimeTemplate(m_FloatBand_Fogs, i, _time));

	return m_Params;
}
