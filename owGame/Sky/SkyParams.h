#pragma once

#include "DBC/DBC__Storage.h"

class CSkyParams
{
public:
	CSkyParams();
	virtual ~CSkyParams();

	const CSkyParams&                                operator+=(const CSkyParams& _s);
	const CSkyParams&                                operator*=(float _weight);

	void                                            Clear();

	void                                            SetColor(uint32 ColorType, vec3 Value);
	vec3                                            GetColor(uint32 ColorType) const;

	void                                            SetFog(uint32 FogType, float Value);
	float                                           GetFog(uint32 FogType) const;

	void                                            SetHighlightSky(bool Value);
	bool                                            GetHighlightSky() const;

	void                                            SetSkybox(const std::shared_ptr<DBC_LightSkyboxRecord>& Value);
	std::shared_ptr<DBC_LightSkyboxRecord>          GetSkybox() const;

	void                                            SetGlow(float GlowValue);
	float                                           GetGlow() const;

	void                                            SetWaterAplha(uint32 WaterAlphaType, float Value);
	float                                           GetWaterAplha(uint32 WaterAlphaType) const;


private:
	vec3                                            m_Colors[LightColors::COUNT];
	float                                           m_Fogs[LightFogs::COUNT];

	bool					                        m_highlightSky;
	std::shared_ptr<DBC_LightSkyboxRecord>          m_SkyBox;
	float                                           m_glow;
	float                                           m_WaterAplha[LightWaterAlpha::COUNT];

	float                                           m_waterShallowAlpha;
	float                                           m_waterDeepAlpha;
	float                                           m_oceanShallowAlpha;
	float                                           m_oceanDeepAlpha;
};