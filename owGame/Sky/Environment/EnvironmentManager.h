#pragma once

#include "DayNightCycle.h"

class EnvironmentManager 
{
public:
	EnvironmentManager(IBaseManager* BaseManager);
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