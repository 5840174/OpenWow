#pragma once

#include "DayNightPhase.h"
#include "../../WowTime.h"

class __declspec(uuid("37A85804-F8D2-44BA-900B-102BDCAF655F")) ZN_API EnvironmentManager
	: public IManager
{
public:
	EnvironmentManager(IBaseManager& BaseManager);
	virtual ~EnvironmentManager();

	void outdoorLighting();
	void SetAmbientLights(bool on);
	void SetFog();

public:
	glm::vec4 m_OutdoorAmbientColor;
	glm::vec4 m_OutdoorDayDiffuseColor;
	glm::vec4 m_OutdoorNightDiffuseColor;
	glm::vec4 m_OutdoorSpecularColor;
	
public:
	bool m_HasSky;

private:
	void LoadDayNightPhases();
	DayNightPhase GetDayNightPhase();

private:
	DayNightPhase dayNightPhase;
	std::vector<DayNightPhase> m_DayNightPhases;
	WowTime m_Time;

private:
	IBaseManager& m_BaseManager;
};