#include "stdafx.h"

// General
#include "MapM2Instance.h"

// Additional

CMapM2Instance::CMapM2Instance(std::string _m2Name) :
	CM2_Base_Instance(_m2Name)
{}

CMapM2Instance::~CMapM2Instance()
{}

void CMapM2Instance::Initialize(const ADT_MDXDef & _placementInfo)
{
    m_UniqueId = _placementInfo.uniqueId;

    // CTransformComponent
    {
        std::shared_ptr<CTransformComponent3D> transformComponent = GetComponent<CTransformComponent3D>();

        transformComponent->SetTranslate(_placementInfo.position);
        vec3 rotate = glm::radians(_placementInfo.rotation);
        rotate.x = -rotate.x;
        rotate.y = rotate.y - glm::half_pi<float>();
        transformComponent->SetRotation(vec3(rotate.z, rotate.y, rotate.x));
        transformComponent->SetScale(vec3(static_cast<float>(_placementInfo.scale) / 1024.0f));
    }
}


//
// SceneNode3D
//
bool CMapM2Instance::Accept(IVisitor& visitor)
{
	const AbstractPass& visitorAsBasePass = reinterpret_cast<AbstractPass&>(visitor);
	const Camera* camera = visitorAsBasePass.GetRenderEventArgs()->Camera;
	
	const CRenderPass_M2* passAsM2Pass = dynamic_cast<const CRenderPass_M2*>(&visitor);
	if (passAsM2Pass)
	{
		if (m_AlreadyDraw.find(m_UniqueId) != m_AlreadyDraw.end())
		{
			//return false;
		}
	}

	/*float distToCamera2D = (camera.GetTranslation() - GetBounds().getCenter()).length() - GetBounds().getRadius();
	if (distToCamera2D > m_QualitySettings.ADT_MCNK_Distance)
	{
		return;
	}*/

	// SceneNode3D
	if (CM2_Base_Instance::Accept(visitor))
	{
		if (passAsM2Pass)
		{
			m_AlreadyDraw.insert(m_UniqueId);
		}
		return true;
	}

	return false;
}

//
std::set<uint32> CMapM2Instance::m_AlreadyDraw;
void CMapM2Instance::reset()
{
	m_AlreadyDraw.clear();
}