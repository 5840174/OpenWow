#pragma once

#include "DBC/DBC__Storage.h"

enum class LightParamsNames : uint32
{
	ParamsClear, 	// Used in clear weather.
	ParamsClearWat, // Used in clear weather while being underwater.
	ParamsStorm,    // Used in rainy / snowy / sandstormy weather.
	ParamsStormWat, // Used in rainy / snowy / sandstormy weather while being underwater.
	ParamsDeath,	// Only 4 and in newer ones 3 are used as value here(with some exceptions).Changing this seems to have no effect in 3.3.5a(is death light setting hardcoded ? )
	ParamsUnk1, 	// Following 3 Params are used only in WotLK records, and not very commonly.Perhaps used in different phases.
	ParamsUnk2,
	ParamsUnk3
};

#include "SkyParams.h"

// FORWARD BEGIN
class SkyManager;
// FORWARD END

class Sky
{
	friend class SkyManager;
public: // TYPES
	template<typename T>
	struct SkyParam
	{
		SkyParam(uint32 _time, T _value) : time(_time), value(_value) {}
		uint32 time;
		T value;
	};

public:
	Sky();
	Sky(const CDBCStorage* DBCStorage, const std::shared_ptr<DBC_LightRecord>& LightData);
	virtual ~Sky();

	void                                            LoadParams(const CDBCStorage* DBCStorage, LightParamsNames _param);

	CSkyParams&                                      Interpolate(uint32 _time);

private:
	std::shared_ptr<DBC_LightRecord>                m_LightRecord;

	vec3					                        m_Position;
	CRange					                        m_Range;

	float					                        m_Wight;
	bool					                        m_IsGlobalSky;

	CSkyParams				                        m_Params;
	std::vector<SkyParam<vec3>>                     m_IntBand_Colors[LightColors::COUNT];
	std::vector<SkyParam<float>>                    m_FloatBand_Fogs[LightFogs::COUNT];
};

#include "Sky.inl"
