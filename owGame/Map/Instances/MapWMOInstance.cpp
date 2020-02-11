#include "stdafx.h"

// General
#include "MapWMOInstance.h"

// Additional
#include "WMO\\RenderPass_WMO.h"

CMapWMOInstance::CMapWMOInstance(const CWMO& WMOObject, const ADT_MODF & _placementInfo)
	: CWMO_Base_Instance(WMOObject)
{
	m_UniqueId = _placementInfo.uniqueId;
	uint16 doodadSetIndex = _placementInfo.doodadSetIndex;
	//m_DoodadSetInfo = _wmoObject->m_DoodadsSetInfos[doodadSetIndex];

	// CTransformComponent
	{
		// Translate
		SetTranslate(_placementInfo.position);

		// Rotate
		vec3 rotate = glm::radians(_placementInfo.rotation);
		rotate.x = -rotate.x;
		rotate.y = rotate.y - glm::half_pi<float>();
		SetRotation(vec3(rotate.z, rotate.y, rotate.x));
	}

	// CColliderComponent
	{
		std::shared_ptr<CColliderComponent3D> colliderComponent = GetComponent<CColliderComponent3D>();

		// Bounds
		BoundingBox bbox(_placementInfo.boundingBox.min, _placementInfo.boundingBox.max);
		colliderComponent->SetBounds(bbox);
	}
}

CMapWMOInstance::~CMapWMOInstance()
{
	//Log::Info("ADT_WMO Deleted");
}


void CMapWMOInstance::Accept(IVisitor* visitor)
{
	//CRenderPass_WMO* passAsWMOPass = dynamic_cast<CRenderPass_WMO*>(visitor);
	// (passAsWMOPass == nullptr)
	//	return false;

	//if (m_AlreadyDraw.find(m_UniqueId) != m_AlreadyDraw.end())
	//{
	//	return false;
	//}

	//AbstractPass* visitorAsBasePass = dynamic_cast<AbstractPass*>(visitor);
	//const ICamera* camera = visitorAsBasePass->GetRenderEventArgs()->Camera;

	//if (!GetComponent<CColliderComponent3D>()->CheckDistance2D(camera, GetGroupQuality()->ADT_WMO_Distance))
	//{
	//	return false;
	//}

	//if (!GetComponent<IColliderComponent3D>()->CheckFrustum(camera))
	//{
	//	return false;
	//}

	// ISceneNode
	CWMO_Base_Instance::Accept(visitor);
}

void CMapWMOInstance::DoUpdate(UpdateEventArgs & e)
{
	//if (lastTotalTime != e.TotalTime)
	//{
	//	reset();
	//	lastTotalTime = e.TotalTime;
	//}
}

//

void CMapWMOInstance::reset()
{
	m_AlreadyDraw.clear();
}
std::set<uint32> CMapWMOInstance::m_AlreadyDraw;