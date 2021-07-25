#include "stdafx.h"

// General
#include "SkyManager.h"

namespace
{

	const float C_SkyRadius = 400.0f;
	const uint32 C_SkySegmentsCount = 32;

	//.............................top.............................med.............................medh............................horiz...........................bottom
	const float      C_SkyAngles[] = { 90.0f,                       30.0f,                       15.0f,                       5.0f,                        0.0f,                        -30.0f,                    -90.0f };
	const ESkyColors C_Skycolors[] = { ESkyColors::SKY_COLOR_SKY_0, ESkyColors::SKY_COLOR_SKY_1, ESkyColors::SKY_COLOR_SKY_2, ESkyColors::SKY_COLOR_SKY_3, ESkyColors::SKY_COLOR_SKY_4, ESkyColors::SKY_COLOR_FOG, ESkyColors::SKY_COLOR_FOG };
	const uint32     C_SkycolorsCount = 7;

	inline void rotate(float x0, float y0, float *x, float *y, float angle)
	{
		float xa = *x - x0;
		float ya = *y - y0;
		*x = xa * glm::cos(angle) - ya * glm::sin(angle) + x0;
		*y = xa * glm::sin(angle) + ya * glm::cos(angle) + y0;
	}

}

CSkyManager::CSkyManager(IScene& Scene)
	: CSceneNode(Scene)
{
	dynamic_cast<IObjectPrivate*>(this)->SetGUID(Guid(ObjectTypes::otSceneNode, cSceneNodeSkyInstance, 1u));

	m_Time.Set(11, 45);

	LoadDayNightPhases();
}

CSkyManager::~CSkyManager()
{}

bool CSkyManager::Load(uint32 MapID)
{
	for (const auto& it : GetBaseManager().GetManager<CDBCStorage>()->DBC_Light())
	{
		if (MapID == it->Get_MapID())
		{
			auto sky = MakeShared(Sky, GetBaseManager().GetManager<CDBCStorage>(), it);
			m_MapSkies.push_back(sky);
		}
	}

	std::sort(m_MapSkies.begin(), m_MapSkies.end(), [](const std::shared_ptr<Sky>& lhs, const std::shared_ptr<Sky>& rhs)
	{
		if (lhs->m_IsGlobalSky)
			return false;
		else if (rhs->m_IsGlobalSky)
			return true;
		else
			return lhs->m_Range.max < rhs->m_Range.max;
	});

	if (m_MapSkies.size() > 0 && !m_MapSkies.back()->m_IsGlobalSky)
	{
		Log::Error("Sky: Sky for maps [%d] size [%d] don't have global sky!!!", MapID, m_MapSkies.size());
		m_MapSkies.back()->m_IsGlobalSky = true;
	}

	SetCullStrategy(ECullStrategy::None);

	CreateSkyColorsBuffer();

	//SetState(ILoadable::ELoadableState::Loaded);

	return true;
}



//
// ISkyManager
//
void CSkyManager::CalculateCurrentSky(const glm::vec3& CameraPosition)
{
	if (m_MapSkies.empty())
		return;

	CalculateSkiesWeights(CameraPosition);

	// interpolation
	m_CurrentSkyParams.Clear();
	for (const auto& it : m_MapSkies)
	{
		if (it->m_Wight > 0.0f)
		{
			SSkyParams params = it->Interpolate(m_Time.GetTime());
			params *= it->m_Wight;

			m_CurrentSkyParams += params;
		}
	}

	// Geometry
	std::vector<ColorRGBA> colors;
	for (uint32 h = 0; h < C_SkySegmentsCount; h++)
	{
		for (uint32 v = 0; v < C_SkycolorsCount - 1; v++)
		{
			colors.push_back(ColorRGBA(GetColor(C_Skycolors[v]), 1.0f));
			colors.push_back(ColorRGBA(GetColor(C_Skycolors[v + 1]), 1.0f));
			colors.push_back(ColorRGBA(GetColor(C_Skycolors[v + 1]), 1.0f));
			colors.push_back(ColorRGBA(GetColor(C_Skycolors[v + 1]), 1.0f));
			colors.push_back(ColorRGBA(GetColor(C_Skycolors[v]), 1.0f));
			colors.push_back(ColorRGBA(GetColor(C_Skycolors[v]), 1.0f));
		}
	}

	std::shared_ptr<IBuffer> colorsBufferNew = GetRenderDevice().GetObjectsFactory().CreateVertexBuffer(colors);
	m_SkyColorsBuffer->Copy(colorsBufferNew.get());
}

void CSkyManager::CreateSkyColorsBuffer()
{
	glm::vec3 basepos1[C_SkycolorsCount];
	glm::vec3 basepos2[C_SkycolorsCount];

	std::vector<glm::vec3> vertices;

	for (uint32 h = 0; h < C_SkySegmentsCount; h++)
	{
		for (uint32 i = 0; i < C_SkycolorsCount; i++)
		{
			basepos1[i] = basepos2[i] = glm::vec3(cosf(glm::radians(C_SkyAngles[i])), sinf(glm::radians(C_SkyAngles[i])), 0.0f) * C_SkyRadius;
			rotate(0, 0, &basepos1[i].x, &basepos1[i].z, glm::two_pi<float>() / C_SkySegmentsCount * (h + 0));
			rotate(0, 0, &basepos2[i].x, &basepos2[i].z, glm::two_pi<float>() / C_SkySegmentsCount * (h + 1));
		}

		for (uint32 v = 0; v < C_SkycolorsCount - 1; v++)
		{
			vertices.push_back(basepos1[v]);
			vertices.push_back(basepos1[v + 1]);
			vertices.push_back(basepos2[v + 1]);

			vertices.push_back(basepos2[v + 1]);
			vertices.push_back(basepos2[v]);
			vertices.push_back(basepos1[v]);
		}
	}

	// Vertex buffer
	std::shared_ptr<IBuffer> vertexBuffer = GetRenderDevice().GetObjectsFactory().CreateVertexBuffer(vertices);

	// Colors buffer
	m_SkyColorsBuffer = GetRenderDevice().GetObjectsFactory().CreateVoidVertexBuffer(vertices.data(), vertices.size(), 0, sizeof(glm::vec4));

	// Geometry
	std::shared_ptr<IGeometry> geometry = GetRenderDevice().GetObjectsFactory().CreateGeometry();
	geometry->AddVertexBuffer(BufferBinding("POSITION", 0), vertexBuffer);
	geometry->AddVertexBuffer(BufferBinding("COLOR", 0), m_SkyColorsBuffer);

	// Material
	std::shared_ptr<IModel> model = GetRenderDevice().GetObjectsFactory().CreateModel();
	model->AddConnection(nullptr, geometry);

	GetComponentT<IModelComponent>()->SetModel(model);
}

void CSkyManager::CalculateSkiesWeights(const glm::vec3& CameraPosition)
{
	m_MapSkies.back()->m_Wight = 1.0f;
	_ASSERT(m_MapSkies.back()->m_IsGlobalSky);

	for (int i = m_MapSkies.size() - 2; i >= 0; i--)
	{
		std::shared_ptr<Sky> s = m_MapSkies[i];
		const float dist = glm::length(CameraPosition - s->m_Position);

		if (dist < s->m_Range.min)
		{
			// we're in a sky, zero out the rest
			s->m_Wight = 1.0f;
			for (uint32_t j = i + 1; j < m_MapSkies.size(); j++)
			{
				m_MapSkies[j]->m_Wight = 0.0f;
			}
		}
		else if (dist < s->m_Range.max)
		{
			// we're in an outer area, scale down the other weights
			float r = (dist - s->m_Range.min) / (s->m_Range.max - s->m_Range.min);
			s->m_Wight = 1.0f - r;
			for (uint32_t j = i + 1; j < m_MapSkies.size(); j++)
			{
				m_MapSkies[j]->m_Wight *= r;
			}
		}
		else
		{
			s->m_Wight = 0.0f;
		}
	}
}

void CSkyManager::LoadDayNightPhases()
{
	auto f = GetBaseManager().GetManager<IFilesManager>()->Open("World\\dnc.db");
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
		SDayNightPhase ols(f);
		m_DayNightPhases.push_back(ols);
	}
}

void CSkyManager::CalculateCurrentDayNightPhase()
{
	uint32 hourA = m_Time.GetTime() / 120;
	uint32 hourB = (hourA + 1) % 24;

	const SDayNightPhase& a = m_DayNightPhases[hourA];
	const SDayNightPhase& b = m_DayNightPhases[hourB];

	float r = static_cast<float>(m_Time.GetTime() - (hourA * 120)) / 120.0f;

	m_CurrentDayNightPhase = SDayNightPhase(a, b, r);
}




//
// ISceneNode
//
void CSkyManager::Update(const UpdateEventArgs& e)
{
	//m_Time.Tick();

	CalculateCurrentSky(e.CameraForCulling->GetPosition());
	CalculateCurrentDayNightPhase();

	// Sky is slways player pos
	SetLocalPosition(e.CameraForCulling->GetPosition());
}




#if 0

void EnvironmentManager::outdoorLighting()
{
	//m_OutdoorAmbientColor = vec4(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_GLOBAL_AMBIENT), 1.0f); // BLACK?

	//m_OutdoorDayDiffuseColor = vec4(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_GLOBAL_DIFFUSE) * m_CurrentDayNightPhase.dayIntensity, 1.0f);
	//m_OutdoorNightDiffuseColor = vec4(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_GLOBAL_DIFFUSE) * m_CurrentDayNightPhase.nightIntensity, 1.0f);
	//m_OutdoorSpecularColor = vec4(1.4f, 1.4f, 1.4f, 1.0f);
}

void EnvironmentManager::SetAmbientLights(bool on)
{
	if (on)
	{
		/*vec4 ambient(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_GLOBAL_AMBIENT), 1);
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

		if (m_CurrentDayNightPhase.dayIntensity > 0)
		{
			//glEnable(GL_LIGHT0);
		}
		else
		{
			//glDisable(GL_LIGHT0);
		}

		if (m_CurrentDayNightPhase.nightIntensity > 0)
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
		glm::vec4 ambient(0, 0, 0, 1);
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

		//vec4 fogcolor(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_FOG), 1);
		//glFogfv(GL_FOG_COLOR, fogcolor); // TODO: retreive fogstart and fogend from lights.lit somehow
		//glFogf(GL_FOG_START, fogdist * fogstart);
		//glFogf(GL_FOG_END, fogdist);
		//glEnable(GL_FOG);
	}
	else
	{
		//glDisable(GL_FOG);
		m_QualitySettings.culldistance = m_QualitySettings.MapChunkRenderDistance;
	}*/
}


#endif