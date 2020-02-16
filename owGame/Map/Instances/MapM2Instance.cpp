#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "MapM2Instance.h"

// Additional

CMapM2Instance::CMapM2Instance(const M2& M2Object, const ADT_MDXDef& _placementInfo) 
	: CM2_Base_Instance(M2Object)
	, m_PlacementInfo(_placementInfo)
{}

CMapM2Instance::~CMapM2Instance()
{}



//
// ISceneNode
//
void CMapM2Instance::Initialize()
{
	// CTransformComponent
	{
		SetTranslate(m_PlacementInfo.position);
		vec3 rotate = glm::radians(m_PlacementInfo.rotation);
		rotate.x = -rotate.x;
		rotate.y = rotate.y - glm::half_pi<float>();
		SetRotation(vec3(rotate.z, rotate.y, rotate.x));
		SetScale(vec3(static_cast<float>(m_PlacementInfo.scale) / 1024.0f));
	}

	__super::Initialize();
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

	//CRenderPass_M2* passAsM2Pass = dynamic_cast<CRenderPass_M2*>(visitor);
    //if (passAsM2Pass == nullptr)
    //    return false;

	//if (m_AlreadyDraw.find(m_UniqueId) != m_AlreadyDraw.end())
	//{
	//	return false;
	//}

	//const ICameraComponent3D* camera = passAsM2Pass->GetRenderEventArgs()->Camera;

	//float distToCamera2D = (camera->GetTranslation() - GetComponent<CColliderComponent3D>()->GetBounds().getCenter()).length() - GetComponent<CColliderComponent3D>()->GetBounds().getRadius();
	//if (distToCamera2D > GetGroupQuality()->ADT_MCNK_Distance)
	//{
	//	return false;
	//}

	// ISceneNode
	CM2_Base_Instance::Accept(visitor);
}

void CMapM2Instance::reset()
{
	m_AlreadyDraw.clear();
}
std::unordered_map<uint32, const CMapM2Instance*> CMapM2Instance::m_AlreadyDraw;

#endif