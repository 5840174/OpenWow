#include "stdafx.h"

// General
#include "EnvironmentManager.h"

// Additiobal
#include <ctime>

EnvironmentManager::EnvironmentManager(IBaseManager& BaseManager)
{
	dayNightCycle = std::make_shared<DayNightCycle>(BaseManager);

	// Colors
    m_OutdoorAmbientColor = vec4();
	m_OutdoorDayDiffuseColor = vec4();
	m_OutdoorNightDiffuseColor = vec4();
	m_OutdoorSpecularColor = vec4();
}

EnvironmentManager::~EnvironmentManager()
{
}

void EnvironmentManager::outdoorLighting()
{
	//m_OutdoorAmbientColor = vec4(m_SkyManager->GetColor(LightColors::LIGHT_COLOR_GLOBAL_AMBIENT), 1.0f); // BLACK?

	//m_OutdoorDayDiffuseColor = vec4(m_SkyManager->GetColor(LightColors::LIGHT_COLOR_GLOBAL_DIFFUSE) * dayNightPhase.dayIntensity, 1.0f);
	//m_OutdoorNightDiffuseColor = vec4(m_SkyManager->GetColor(LightColors::LIGHT_COLOR_GLOBAL_DIFFUSE) * dayNightPhase.nightIntensity, 1.0f);
	//m_OutdoorSpecularColor = vec4(1.4f, 1.4f, 1.4f, 1.0f);
}

void EnvironmentManager::SetAmbientLights(bool on)
{
	if (on)
	{
		/*vec4 ambient(m_SkyManager->GetColor(LightColors::LIGHT_COLOR_GLOBAL_AMBIENT), 1);
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

		if (dayNightPhase.dayIntensity > 0)
		{
			//glEnable(GL_LIGHT0);
		}
		else
		{
			//glDisable(GL_LIGHT0);
		}

		if (dayNightPhase.nightIntensity > 0)
		{
			//glEnable(GL_LIGHT1);
		}
		else
		{
			//glDisable(GL_LIGHT1);
		}*/
	}
	else
	{
		vec4 ambient(0, 0, 0, 1);
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		//glDisable(GL_LIGHT0);
		//glDisable(GL_LIGHT1);
	}
}

void EnvironmentManager::SetFog()
{
	/*if (m_QualitySettings.drawfog)
	{
		float fogdist = m_QualitySettings.fogdistance;
		float fogstart = 0.5f;

		m_QualitySettings.culldistance = fogdist;

		//vec4 fogcolor(m_SkyManager->GetColor(LightColors::LIGHT_COLOR_FOG), 1);
		//glFogfv(GL_FOG_COLOR, fogcolor); // TODO: retreive fogstart and fogend from lights.lit somehow
		//glFogf(GL_FOG_START, fogdist * fogstart);
		//glFogf(GL_FOG_END, fogdist);
		//glEnable(GL_FOG);
	}
	else
	{
		//glDisable(GL_FOG);
		m_QualitySettings.culldistance = m_QualitySettings.ADT_MCNK_Distance;
	}*/
}

