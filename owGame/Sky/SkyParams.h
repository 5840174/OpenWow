#pragma once

enum ESkyColors : uint8
{
	SKY_COLOR_GLOBAL_DIFFUSE = 0,
	SKY_COLOR_GLOBAL_AMBIENT,

	SKY_COLOR_SKY_0,
	SKY_COLOR_SKY_1,
	SKY_COLOR_SKY_2,
	SKY_COLOR_SKY_3,
	SKY_COLOR_SKY_4,

	SKY_COLOR_FOG,
	SKY_COLOR_UNK0,
	SKY_COLOR_SUN,
	SKY_COLOR_SUN_HALO,
	SKY_COLOR_UNK1,
	SKY_COLOR_CLOUD,
	SKY_COLOR_UNK2,

	SKY_COLOR_OCEAN_LIGHT,
	SKY_COLOR_OCEAN_DARK,
	SKY_COLOR_RIVER_LIGHT,
	SKY_COLOR_RIVER_DARK,

	SKY_COLOR_COUNT
};

enum ESkyFogs : uint8
{
	SKY_FOG_DISTANCE = 0,    // Fog distance multiplied by 36 - distance at which everything will be hidden by the fog
	SKY_FOG_MULTIPLIER,      // Fog multiplier - fog distance * fog multiplier = fog start distance. 0 - 0, 999...
	SKY_FOG_CELESTIAL_GLOW,  // Celestial Glow Through - the brightness of the sun and moon as it shows through cloud cover.Note that this effect only appears when the Sun or Moon is being obscured by clouds. 0 - 1
	SKY_FOG_CLOUD_DESINTY,   // Cloud Density - Controls the density of cloud cover in the area.Value range is 0.0 to 1.0.
	SKY_FOG_UNK0,
	SKY_FOG_UNK1,

	SKY_FOG_COUNT
};

enum ESkyWaterAlpha : uint8
{
	SKY_WATER_SHALLOW = 0,
	SKY_WATER_DEEP,
	SKY_OCEAN_SHALLOW,
	SKY_OCEAN_DEEP,

	SKY_COUNT
};

enum ESkyParamsNames : uint32
{
	SKY_ParamsClear, 	// Used in clear weather.
	SKY_ParamsClearWat, // Used in clear weather while being underwater.
	SKY_ParamsStorm,    // Used in rainy / snowy / sandstormy weather.
	SKY_ParamsStormWat, // Used in rainy / snowy / sandstormy weather while being underwater.
	SKY_ParamsDeath,	// Only 4 and in newer ones 3 are used as value here(with some exceptions).Changing this seems to have no effect in 3.3.5a(is death light setting hardcoded ? )
	SKY_ParamsUnk1, 	// Following 3 Params are used only in WotLK records, and not very commonly.Perhaps used in different phases.
	SKY_ParamsUnk2,
	SKY_ParamsUnk3
};

class ZN_API CSkyParams
{
public:
	CSkyParams();
	virtual ~CSkyParams();

	const CSkyParams&                                operator+=(const CSkyParams& _s);
	const CSkyParams&                                operator*=(float _weight);

	void                                            Clear();

	void                                            SetColor(uint32 ColorType, glm::vec3 Value);
	glm::vec3                                       GetColor(uint32 ColorType) const;

	void                                            SetFog(uint32 FogType, float Value);
	float                                           GetFog(uint32 FogType) const;

	void                                            SetHighlightSky(bool Value);
	bool                                            GetHighlightSky() const;

	void                                            SetSkybox(const DBC_LightSkyboxRecord* Value);
	const DBC_LightSkyboxRecord*                    GetSkybox() const;

	void                                            SetGlow(float GlowValue);
	float                                           GetGlow() const;

	void                                            SetWaterAplha(uint32 WaterAlphaType, float Value);
	float                                           GetWaterAplha(uint32 WaterAlphaType) const;


private:
	glm::vec3                                       m_Colors[ESkyColors::SKY_COLOR_COUNT];
	float                                           m_Fogs[ESkyFogs::SKY_FOG_COUNT];

	bool					                        m_highlightSky;
	const DBC_LightSkyboxRecord*                    m_SkyBox;
	float                                           m_glow;
	float                                           m_WaterAplha[ESkyWaterAlpha::SKY_COUNT];

	float                                           m_waterShallowAlpha;
	float                                           m_waterDeepAlpha;
	float                                           m_oceanShallowAlpha;
	float                                           m_oceanDeepAlpha;
};