#pragma once

#include "Sky.h"

class ZN_API SkyManager
	: public CSceneNode
	, public IManager
{
public:
	ZN_OBJECTCLASS(cWowSkyManagerCustom)

	SkyManager(IScene& Scene);
	virtual ~SkyManager();

	bool                                            Load(uint32 MapID);

	// ISceneNode
	void                                            Update(const UpdateEventArgs& e) override;

	// ISkyManager
	void                                            Calculate(const ICameraComponent3D* camera, uint32 _time);
	bool                                            HasSkies() const { return !skies.empty(); }
	glm::vec3                                       GetColor(ESkyColors _color) const { return m_Interpolated.GetColor(_color); }
	float                                           GetFog(ESkyFogs _fog) const { return m_Interpolated.GetFog(_fog); }
	float                                           GetGlow() const { return m_Interpolated.GetGlow(); }
	float                                           GetWaterShallowAlpha() const { return m_Interpolated.GetWaterAplha(ESkyWaterAlpha::SKY_WATER_SHALLOW); }
	float                                           GetWaterDarkAlpha() const { return m_Interpolated.GetWaterAplha(ESkyWaterAlpha::SKY_WATER_DEEP); }
	float                                           GetOceanShallowAlpha() const { return m_Interpolated.GetWaterAplha(ESkyWaterAlpha::SKY_OCEAN_SHALLOW); }
	float                                           GetOceanDarkAlpha() const { return m_Interpolated.GetWaterAplha(ESkyWaterAlpha::SKY_OCEAN_DEEP); }

private:
	void                                            InitBuffer();
	void                                            CalculateSkiesWeights(const glm::vec3& pos);

private:
	CSkyParams                                      m_Interpolated;
	std::shared_ptr<IBuffer>                        colorsBuffer;
	std::vector<std::shared_ptr<Sky>>               skies;
};

