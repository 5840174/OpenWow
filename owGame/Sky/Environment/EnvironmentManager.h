#pragma once

#include "DayNightCycle.h"

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
	vec4 m_OutdoorAmbientColor;
	vec4 m_OutdoorDayDiffuseColor;
	vec4 m_OutdoorNightDiffuseColor;
	vec4 m_OutdoorSpecularColor;
	
public:
	bool m_HasSky;


	std::shared_ptr<DayNightCycle> dayNightCycle;
	DayNightPhase dayNightPhase;
};