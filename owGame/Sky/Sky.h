#pragma once

#include "SkyParams.h"

// FORWARD BEGIN
class CSkyManager;
// FORWARD END

class Sky
{
	friend class CSkyManager;

public: // TYPES
	template<typename T>
	struct SSkyInterpolatedParam
	{
		SSkyInterpolatedParam(uint32 _time, T _value) 
			: time(_time)
			, value(_value) 
		{}

		uint32 time;
		T value;
	};

public:
	Sky();
	Sky(const CDBCStorage* DBCStorage, const DBC_LightRecord* LightData);
	virtual ~Sky();

	void                             LoadParams(const CDBCStorage* DBCStorage, ESkyParamsNames _param);

	SSkyParams&                      Interpolate(uint32 _time);

private:
	const DBC_LightRecord*           m_LightRecord;

	glm::vec3					     m_Position;
	CRange					         m_Range;

	float                            m_Wight;
	bool                             m_IsGlobalSky;

	SSkyParams                       m_Params;
	std::vector<SSkyInterpolatedParam<ColorRGB>>  m_IntBand_Colors[ESkyColors::SKY_COLOR_COUNT];
	std::vector<SSkyInterpolatedParam<float>>     m_FloatBand_Fogs[ESkyFogs::SKY_FOG_COUNT];
};
