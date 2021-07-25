#pragma once

#include "Sky.h"
#include "DayNightPhase.h"

class ZN_API CSkyManager
	: public CSceneNode
	, public IManager
{
public:
	ZN_OBJECTCLASS(cWowSkyManagerCustom)

	CSkyManager(IScene& Scene);
	virtual ~CSkyManager();

	bool                                            Load(uint32 MapID);

	// ISceneNode
	void                                            Update(const UpdateEventArgs& e) override;

	// ISkyManager
	bool                                            HasSkies() const { return false == m_MapSkies.empty(); }
	ColorRGB                                        GetColor(ESkyColors SkyColorType) const { return m_CurrentSkyParams.Colors[SkyColorType]; }
	float                                           GetFog(ESkyFogs SkyFogType) const { return m_CurrentSkyParams.Fogs[SkyFogType]; }
	float                                           GetWaterAlpha(ESkyWaterAlpha SkyWaterAlphaType) const { return m_CurrentSkyParams.WaterAplha[SkyWaterAlphaType]; }
	float                                           GetGlow() const { return m_CurrentSkyParams.Glow; }

	const SDayNightPhase&                           GetDNPhase() const { return m_CurrentDayNightPhase; }

private:
	void                                            CreateSkyColorsBuffer();
	void                                            CalculateCurrentSky(const glm::vec3& CameraPosition);
	void                                            CalculateSkiesWeights(const glm::vec3& CameraPosition);

	void											LoadDayNightPhases();
	void                                            CalculateCurrentDayNightPhase();


private:
	SSkyParams                                      m_CurrentSkyParams;
	std::shared_ptr<IBuffer>                        m_SkyColorsBuffer;
	std::vector<std::shared_ptr<Sky>>               m_MapSkies;

	SDayNightPhase                                  m_CurrentDayNightPhase;
	std::vector<SDayNightPhase>                     m_DayNightPhases;

	WowTime                                         m_Time;

};

