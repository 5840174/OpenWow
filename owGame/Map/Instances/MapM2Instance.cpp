#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "MapM2Instance.h"

// Additional

CMapM2Instance::CMapM2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object, const SMapTile_MDDF& _placementInfo) 
	: CM2_Base_Instance(Scene, M2Object)
	, m_PlacementInfo(_placementInfo)
{
	SetName("CMapM2Instance: " + M2Object->getFilename());
}

CMapM2Instance::~CMapM2Instance()
{
	//Log::Info("CMapM2Instance: '%s' deleted.", GetName().c_str());
}



//
// ISceneNode
//
void CMapM2Instance::Initialize()
{
	__super::Initialize();

	SetLocalPosition(m_PlacementInfo.position);
	SetLocalRotationEuler(m_PlacementInfo.rotation);
	SetLocalScale(glm::vec3(static_cast<float>(m_PlacementInfo.scale) / 1024.0f));

	if (auto colliderComponent = GetComponentT<IColliderComponent>())
	{
		colliderComponent->SetCullDistance(GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("MapTileM2RenderDistance")->Get());
	}
}

void CMapM2Instance::Accept(IVisitor* visitor)
{
	const auto& idIter = m_AlreadyDraw.find(m_PlacementInfo.uniqueId);
	if (idIter != m_AlreadyDraw.end())
	{
		if (idIter->second != this)
			return;
	}
	else
	{
		m_AlreadyDraw.insert(std::make_pair(m_PlacementInfo.uniqueId, this));
	}

	__super::Accept(visitor);
}



//
// Protected
//
glm::mat4 CMapM2Instance::CalculateLocalTransform() const
{
	glm::mat4 localTransform(1.0f);
	localTransform = glm::translate(localTransform, m_PlacementInfo.position);
	localTransform *= glm::eulerAngleYZX(glm::radians(m_PlacementInfo.rotation.y - 90.0f), glm::radians(-m_PlacementInfo.rotation.x), glm::radians(m_PlacementInfo.rotation.z));
	localTransform = glm::scale(localTransform, glm::vec3(static_cast<float>(m_PlacementInfo.scale) / 1024.0f));
	return localTransform;
}


void CMapM2Instance::reset()
{
	m_AlreadyDraw.clear();
}
std::map<uint32, const CMapM2Instance*> CMapM2Instance::m_AlreadyDraw;

#endif