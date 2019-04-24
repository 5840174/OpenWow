#include "stdafx.h"

// General
#include "MapWMOInstance.h"

// Additional
#include "WMO\\RenderPass_WMO.h"

CMapWMOInstance::CMapWMOInstance(std::string _wmoName) :
	CWMO_Base_Instance(_wmoName)
{

}

CMapWMOInstance::~CMapWMOInstance()
{
	//Log::Info("ADT_WMO Deleted");
}

void CMapWMOInstance::Initialize(const ADT_MODF & _placementInfo)
{
    m_UniqueId = _placementInfo.uniqueId;
    uint16 doodadSetIndex = _placementInfo.doodadSetIndex;
    //m_DoodadSetInfo = _wmoObject->m_DoodadsSetInfos[doodadSetIndex];

    // CTransformComponent
    {
        std::shared_ptr<CTransformComponent> transformComponent = GetComponent<CTransformComponent>();

        // Translate
        transformComponent->SetTranslate(_placementInfo.position);

        // Rotate
        vec3 rotate = glm::radians(_placementInfo.rotation);
        rotate.x = -rotate.x;
        rotate.y = rotate.y - glm::half_pi<float>();
        transformComponent->SetRotation(vec3(rotate.z, rotate.y, rotate.x));
    }

    // CColliderComponent
    {
        std::shared_ptr<CColliderComponent> colliderComponent = GetComponent<CColliderComponent>();

        // Bounds
        BoundingBox bbox(_placementInfo.boundingBox.min, _placementInfo.boundingBox.max);
        colliderComponent->SetBounds(bbox);
    }
}

bool CMapWMOInstance::Accept(IVisitor& visitor)
{
	const CRenderPass_WMO* passAsWMOPass = dynamic_cast<const CRenderPass_WMO*>(&visitor);
	if (passAsWMOPass)
	{
		if (m_AlreadyDraw.find(m_UniqueId) != m_AlreadyDraw.end())
		{
			return false;
		}
	}

	// SceneNode3D
	if (CWMO_Base_Instance::Accept(visitor))
	{
		if (passAsWMOPass)
		{
			m_AlreadyDraw.insert(m_UniqueId);
		}
		return true;
	}

	return false;
}

//

void CMapWMOInstance::reset()
{
	m_AlreadyDraw.clear();
}
std::set<uint32> CMapWMOInstance::m_AlreadyDraw;