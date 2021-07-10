#include "stdafx.h"

// Include
#include "SkyManager.h"

// General
#include "Sky.h"

namespace
{
	const float cSkyMultiplier = 36.0f;

	template<typename T>
	inline T GetByTimeTemplate(std::vector<Sky::SSkyInterpolatedParam<T>> param, uint32 _time)
	{
		if (param.empty())
			return T();

		T parBegin, parEnd;
		uint32 timeBegin, timeEnd;
		uint32_t last = static_cast<uint32>(param.size()) - 1;

		if (_time < param[0].time)
		{
			// interpolate between last and first
			parBegin = param[last].value;
			timeBegin = param[last].time;
			parEnd = param[0].value;
			timeEnd = param[0].time + C_Game_SecondsInDay; // next day
			_time += C_Game_SecondsInDay;
		}
		else
		{
			for (uint32 i = last; i >= 0; i--)
			{
				if (_time >= param[i].time)
				{
					parBegin = param[i].value;
					timeBegin = param[i].time;

					if (i == last) // if current is last, then interpolate with first
					{
						parEnd = param[0].value;
						timeEnd = param[0].time + C_Game_SecondsInDay;
					}
					else
					{
						parEnd = param[i + 1].value;
						timeEnd = param[i + 1].time;
					}
					break;
				}
			}
		}

		float tt = (float)(_time - timeBegin) / (float)(timeEnd - timeBegin);
		return parBegin * (1.0f - tt) + (parEnd * tt);
	}

}

Sky::Sky() :
	m_LightRecord(nullptr)
{}

Sky::Sky(const CDBCStorage* DBCStorage, const DBC_LightRecord* LightData)
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

	LoadParams(DBCStorage, ESkyParamsNames::SKY_ParamsClear);
}

Sky::~Sky()
{}


class SkyParam_Color 
	: public Sky::SSkyInterpolatedParam<ColorRGB>
{
public:
	SkyParam_Color(uint32 _time, uint32 _color)
		: SSkyInterpolatedParam<ColorRGB>(_time, fromRGB(_color))
	{}
};


class SkyParam_Fog 
	: public Sky::SSkyInterpolatedParam<float>
{
public:
	SkyParam_Fog(uint32 _time, float _param)
		: SSkyInterpolatedParam<float>(_time, _param)
	{}
};

void Sky::LoadParams(const CDBCStorage* DBCStorage, ESkyParamsNames _param)
{
	for (uint32 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
		m_IntBand_Colors[i].clear();
	for (uint32 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
		m_FloatBand_Fogs[i].clear();

	const DBC_LightParamsRecord* paramRecord = DBCStorage->DBC_LightParams()[m_LightRecord->Get_LightParams(_param)];
	_ASSERT(paramRecord != nullptr);
	uint32 paramSet = paramRecord->Get_ID();

	//-- LightParams
	m_Params.HighlightSky = paramRecord->Get_HighlightSky();
	m_Params.LightSkyBoxRecord = DBCStorage->DBC_LightSkybox()[paramRecord->Get_LightSkyboxID()];
	m_Params.Glow = paramRecord->Get_Glow();


	if (m_Params.LightSkyBoxRecord != nullptr)
	{
		// TODO
		//Log::Info("Sky: Skybox name = '%s'.", m_Params.GetSkybox()->Get_Filename().c_str());
	}

	//-- Color params
	for (uint32 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
	{
		const DBC_LightIntBandRecord* lightColorsRecord = DBCStorage->DBC_LightIntBand()[paramSet * ESkyColors::SKY_COLOR_COUNT - (ESkyColors::SKY_COLOR_COUNT - 1) + i];
		_ASSERT(lightColorsRecord != nullptr);
		for (uint32 l = 0; l < lightColorsRecord->Get_Count(); l++)
		{
			// Read time & color value
			m_IntBand_Colors[i].push_back(SkyParam_Color(lightColorsRecord->Get_Times(l), lightColorsRecord->Get_Values(l)));
		}
	}

	//-- Fog, Sun, Clouds param
	for (uint32 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
	{
		const DBC_LightFloatBandRecord* lightFogRecord = DBCStorage->DBC_LightFloatBand()[paramSet * ESkyFogs::SKY_FOG_COUNT - (ESkyFogs::SKY_FOG_COUNT - 1) + i];
		_ASSERT(lightFogRecord != nullptr);
		for (uint32 l = 0; l < lightFogRecord->Get_Count(); l++)
		{
			// Read time & fog param
			float param = lightFogRecord->Get_Values(l);
			if (i == ESkyFogs::SKY_FOG_DISTANCE)
				param /= cSkyMultiplier;
			m_FloatBand_Fogs[i].push_back(SkyParam_Fog(lightFogRecord->Get_Times(l), param));
		}
	}

	m_Params.WaterAplha[ESkyWaterAlpha::SKY_WATER_SHALLOW] = paramRecord->Get_WaterShallowAlpha();
	m_Params.WaterAplha[ESkyWaterAlpha::SKY_WATER_DEEP] = paramRecord->Get_WaterDeepAlpha();
	m_Params.WaterAplha[ESkyWaterAlpha::SKY_OCEAN_SHALLOW] = paramRecord->Get_OceanShallowAlpha();
	m_Params.WaterAplha[ESkyWaterAlpha::SKY_OCEAN_DEEP] = paramRecord->Get_OceanDeepAlpha();
}

SSkyParams& Sky::Interpolate(uint32 _time)
{
	for (uint8 i = 0; i < ESkyColors::SKY_COLOR_COUNT; i++)
		m_Params.Colors[i] = GetByTimeTemplate(m_IntBand_Colors[i], _time);

	for (uint8 i = 0; i < ESkyFogs::SKY_FOG_COUNT; i++)
		m_Params.Fogs[i] = GetByTimeTemplate(m_FloatBand_Fogs[i], _time);

	return m_Params;
}
