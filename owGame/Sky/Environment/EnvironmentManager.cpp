#include "stdafx.h"

// General
#include "EnvironmentManager.h"

// Additiobal
#include <ctime>

EnvironmentManager::EnvironmentManager(IBaseManager& BaseManager)
	: m_BaseManager(BaseManager)
{
	m_Time.Set(11, 0);
	LoadDayNightPhases();

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

void EnvironmentManager::LoadDayNightPhases()
{
	std::shared_ptr<IFile> f = m_BaseManager.GetManager<IFilesManager>()->Open("World\\dnc.db");
	if (f == nullptr)
	{
		Log::Error("DayNightCycle[]: Can't init day-night cycle.");
		return;
	}

	uint32 nFields1, nFields;

	// Header
	f->readBytes(&nFields, 4);
	f->readBytes(&nFields1, 4);
	_ASSERT(nFields == nFields1);
	_ASSERT(nFields == 25);

	// Field Descriptions
	uint32 magic0x53;
	f->readBytes(&magic0x53, 4);
	_ASSERT(magic0x53 == 0x53);

	// Final offset
	uint32 d;
	f->readBytes(&d, 4); // d is now the final offset

	// Skip names
	f->seek(8 + nFields * 8);

	while (f->getPos() < d)
	{
		DayNightPhase ols(f);
		m_DayNightPhases.push_back(ols);
	}
}

DayNightPhase EnvironmentManager::GetDayNightPhase()
{
	uint32 hourA = m_Time.GetTime() / 120;
	uint32 hourB = (hourA + 1) % 24;

	DayNightPhase* a = &m_DayNightPhases[hourA];
	DayNightPhase* b = &m_DayNightPhases[hourB];

	float r = static_cast<float>(m_Time.GetTime() - (hourA * 120)) / 120.0f;

	return DayNightPhase(a, b, r);
}