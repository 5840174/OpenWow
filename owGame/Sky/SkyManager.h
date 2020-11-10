#pragma once

#include "Sky.h"

class ZN_API SkyManager
	: public CSceneNode
	, public ISkyManager
	, public CLoadableObject
{
public:
	SkyManager(IRenderDevice& RenderDevice);
	virtual ~SkyManager();

	// ILoadableObject
	bool                                            Load(uint32 MapID);

	// SceneNode3D
	void                                            Update(const UpdateEventArgs& e) override;

	// ISkyManager
	void                                            Calculate(const ICameraComponent3D* camera, uint32 _time) override;
	bool                                            HasSkies() const override { return !skies.empty(); }
	glm::vec3                                       GetColor(LightColors::List _color) const override { return m_Interpolated.GetColor(_color); }
	float                                           GetFog(LightFogs::List _fog) const override { return m_Interpolated.GetFog(_fog); }
	float                                           GetGlow() const override { return m_Interpolated.GetGlow(); }
	float                                           GetWaterShallowAlpha() const override { return m_Interpolated.GetWaterAplha(LightWaterAlpha::WATER_SHALLOW); }
	float                                           GetWaterDarkAlpha() const override { return m_Interpolated.GetWaterAplha(LightWaterAlpha::WATER_DEEP); }
	float                                           GetOceanShallowAlpha() const override { return m_Interpolated.GetWaterAplha(LightWaterAlpha::OCEAN_SHALLOW); }
	float                                           GetOceanDarkAlpha() const override { return m_Interpolated.GetWaterAplha(LightWaterAlpha::OCEAN_DEEP); }

private:
	void                                            InitBuffer();
	void                                            CalculateSkiesWeights(const glm::vec3& pos);

private:
	CSkyParams                                      m_Interpolated;
	std::shared_ptr<IBuffer>                        colorsBuffer;
	std::vector<std::shared_ptr<Sky>>               skies;

private:
	IRenderDevice& m_RenderDevice;
};

