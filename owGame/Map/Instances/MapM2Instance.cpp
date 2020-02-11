#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "MapM2Instance.h"

// Additional

CMapM2Instance::CMapM2Instance(const M2& M2Object, const ADT_MDXDef& _placementInfo) 
	: CM2_Base_Instance(M2Object)
{
	m_UniqueId = _placementInfo.uniqueId;

	// CTransformComponent
	{
		SetTranslate(_placementInfo.position);
		vec3 rotate = glm::radians(_placementInfo.rotation);
		rotate.x = -rotate.x;
		rotate.y = rotate.y - glm::half_pi<float>();
		SetRotation(vec3(rotate.z, rotate.y, rotate.x));
		SetScale(vec3(static_cast<float>(_placementInfo.scale) / 1024.0f));
	}
}

CMapM2Instance::~CMapM2Instance()
{}


//
// ISceneNode
//
void CMapM2Instance::Accept(IVisitor* visitor)
{
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


void CMapM2Instance::DoUpdate(UpdateEventArgs & e)
{
	//if (lastTotalTime22 != e.TotalTime)
	//{
	//	reset();
	//}
}

//
std::set<uint32> CMapM2Instance::m_AlreadyDraw;
void CMapM2Instance::reset()
{
	m_AlreadyDraw.clear();
}

#endif