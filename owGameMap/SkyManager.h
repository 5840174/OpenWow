#pragma once

#include "Sky.h"

// FORWARD BEGIN
class CMap;
// FORWARD END

class SkyManager 
	: public CSceneNodeProxie
	, public CLoadableObject
	, public ISkyManager
{
public:
	                                                SkyManager();
	virtual                                         ~SkyManager();

	// SceneNode3D
	void                                            UpdateCamera(const ICamera* camera) override;

    // ILoadableObject
    bool                                            Load() override;

	// ISkyManager
	void                                            Calculate(const ICamera* camera, uint32 _time) override;
	bool                                            HasSkies() const override { return !skies.empty(); }
	vec3                                            GetColor(LightColors::List _color) const override { return m_Interpolated.GetColor(_color); }
	float                                           GetFog(LightFogs::List _fog) const override { return m_Interpolated.GetFog(_fog); }
	float                                           GetGlow() const override { return m_Interpolated.GetGlow(); }
    float                                           GetWaterShallowAlpha() const override { return m_Interpolated.GetWaterAplha(LightWaterAlpha::WATER_SHALLOW); }
	float                                           GetWaterDarkAlpha() const override { return m_Interpolated.GetWaterAplha(LightWaterAlpha::WATER_DEEP); }
	float                                           GetOceanShallowAlpha() const override { return m_Interpolated.GetWaterAplha(LightWaterAlpha::OCEAN_SHALLOW); }
	float                                           GetOceanDarkAlpha() const override { return m_Interpolated.GetWaterAplha(LightWaterAlpha::OCEAN_DEEP); }

protected:
    std::shared_ptr<CMap>                           GetMapController() const;

private:
	void                                            InitBuffer();
	void                                            CalculateSkiesWeights(cvec3 pos);

private:
	CSkyParams                                      m_Interpolated;

	std::shared_ptr<IBuffer>                        colorsBuffer;

	std::vector<std::shared_ptr<Sky>>               skies;
};

