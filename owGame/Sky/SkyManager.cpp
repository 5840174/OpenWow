#include "stdafx.h"

// General
#include "SkyManager.h"

const float C_SkyRadius = 400.0f;
const uint32 C_SkySegmentsCount = 32;

//.............................top.............................med.............................medh............................horiz...........................bottom
const float  C_SkyAngles[] = { 90.0f,                          30.0f,                          15.0f,                          5.0f,                           0.0f,                           -30.0f,                       -90.0f                       };
const uint32 C_Skycolors[] = { LightColors::LIGHT_COLOR_SKY_0, LightColors::LIGHT_COLOR_SKY_1, LightColors::LIGHT_COLOR_SKY_2, LightColors::LIGHT_COLOR_SKY_3, LightColors::LIGHT_COLOR_SKY_4, LightColors::LIGHT_COLOR_FOG, LightColors::LIGHT_COLOR_FOG };
const uint32 C_SkycolorsCount = 7;

SkyManager::SkyManager(IScene& Scene)
	: CSceneNode(Scene)
{
}

SkyManager::~SkyManager()
{}

bool SkyManager::Load(uint32 MapID)
{
	for (const auto& it : GetBaseManager().GetManager<CDBCStorage>()->DBC_Light())
	{
		if (MapID == it->Get_MapID())
		{
			std::shared_ptr<Sky> sky = std::make_shared<Sky>(GetBaseManager().GetManager<CDBCStorage>(), it);
			skies.push_back(sky);
		}
	}

	std::sort(skies.begin(), skies.end(), [](const std::shared_ptr<Sky>& lhs, const std::shared_ptr<Sky>& rhs)
	{
		if (lhs->m_IsGlobalSky)
			return false;
		else if (rhs->m_IsGlobalSky)
			return true;
		else
			return lhs->m_Range.max < rhs->m_Range.max;
	});

	if (skies.size() > 0 && !skies.back()->m_IsGlobalSky)
	{
		Log::Error("Sky: Sky for maps [%d] size [%d] don't have global sky!!!", MapID, skies.size());
		skies.back()->m_IsGlobalSky = true;
	}

	if (auto colliderComponent = GetComponentT<IColliderComponent>())
		colliderComponent->SetCullStrategy(IColliderComponent::ECullStrategy::None);

	InitBuffer();

	//SetState(ILoadable::ELoadableState::Loaded);

	return true;
}



//
// SceneNode3D
//
void SkyManager::Update(const UpdateEventArgs& e)
{
	//if (GetState() != ILoadable::ELoadableState::Loaded)
	//	return;

	Calculate(e.CameraForCulling, 1440/*GetMapController()->getTime()->GetTime()*/);

	SetPosition(e.CameraForCulling->GetPosition());
}



//
// ISkyManager
//
void SkyManager::Calculate(const ICameraComponent3D* camera, uint32 _time)
{
	//if (GetState() != ILoadable::ELoadableState::Loaded)
	//	return;

	CalculateSkiesWeights(camera->GetPosition());

	// interpolation
	m_Interpolated.Clear();
	for (const auto& it : skies)
	{
		if (it->m_Wight > 0.0f)
		{
			CSkyParams params = it->Interpolate(_time);
			params *= it->m_Wight;

			m_Interpolated += params;
		}
	}

	// Geometry
	std::vector<glm::vec4> colors;
	for (uint32 h = 0; h < C_SkySegmentsCount; h++)
	{
		for (uint32 v = 0; v < C_SkycolorsCount - 1; v++)
		{
			colors.push_back(glm::vec4(m_Interpolated.GetColor(C_Skycolors[v]), 0.0f));
			colors.push_back(glm::vec4(m_Interpolated.GetColor(C_Skycolors[v + 1]), 0.0f));
			colors.push_back(glm::vec4(m_Interpolated.GetColor(C_Skycolors[v + 1]), 0.0f));
			colors.push_back(glm::vec4(m_Interpolated.GetColor(C_Skycolors[v + 1]), 0.0f));
			colors.push_back(glm::vec4(m_Interpolated.GetColor(C_Skycolors[v]), 0.0f));
			colors.push_back(glm::vec4(m_Interpolated.GetColor(C_Skycolors[v]), 0.0f));
		}
	}

	std::shared_ptr<IBuffer> colorsBufferNew = GetRenderDevice().GetObjectsFactory().CreateVertexBuffer(colors);
	colorsBuffer->Copy(colorsBufferNew.get());
}

namespace
{
inline void rotate(float x0, float y0, float *x, float *y, float angle)
{
	float xa = *x - x0;
	float ya = *y - y0;
	*x = xa * glm::cos(angle) - ya * glm::sin(angle) + x0;
	*y = xa * glm::sin(angle) + ya * glm::cos(angle) + y0;
}
}

void SkyManager::InitBuffer()
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
	colorsBuffer = GetRenderDevice().GetObjectsFactory().CreateVoidVertexBuffer(vertices.data(), vertices.size(), 0, sizeof(glm::vec4));

	// Geometry
	std::shared_ptr<IGeometry> geometry = GetRenderDevice().GetObjectsFactory().CreateGeometry();
	geometry->AddVertexBuffer(BufferBinding("POSITION", 0), vertexBuffer);
	geometry->AddVertexBuffer(BufferBinding("COLOR", 0), colorsBuffer);

	// Material
	std::shared_ptr<IModel> model = GetRenderDevice().GetObjectsFactory().CreateModel();
	model->AddConnection(nullptr, geometry);

	GetComponentT<IModelComponent>()->SetModel(model);
}

void SkyManager::CalculateSkiesWeights(const glm::vec3& pos)
{
	skies.back()->m_Wight = 1.0f;
	_ASSERT(skies.back()->m_IsGlobalSky);

	for (int i = skies.size() - 2; i >= 0; i--)
	{
		std::shared_ptr<Sky> s = skies[i];
		const float dist = glm::length(pos - s->m_Position);

		if (dist < s->m_Range.min)
		{
			// we're in a sky, zero out the rest
			s->m_Wight = 1.0f;
			for (uint32_t j = i + 1; j < skies.size(); j++)
			{
				skies[j]->m_Wight = 0.0f;
			}
		}
		else if (dist < s->m_Range.max)
		{
			// we're in an outer area, scale down the other weights
			float r = (dist - s->m_Range.min) / (s->m_Range.max - s->m_Range.min);
			s->m_Wight = 1.0f - r;
			for (uint32_t j = i + 1; j < skies.size(); j++)
			{
				skies[j]->m_Wight *= r;
			}
		}
		else
		{
			s->m_Wight = 0.0f;
		}
	}
}