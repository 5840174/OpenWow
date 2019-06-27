#include "stdafx.h"

// General
#include "WMO_Base_Instance.h"

CWMO_Base_Instance::CWMO_Base_Instance(std::string _wmoName) 
    : m_WMOName(_wmoName)
    , m_QualitySettings(GetSettingsGroup<CGroupQuality>())
{}

CWMO_Base_Instance::~CWMO_Base_Instance()
{}


void CWMO_Base_Instance::CreateInstances()
{
	m_WMO->CreateInsances(std::static_pointer_cast<CWMO_Base_Instance, SceneNode3D>(shared_from_this()));

    std::shared_ptr<CTransformComponent3D> transformComponent = GetComponent<CTransformComponent3D>();

	if (m_WMO->m_PortalController != nullptr)
	{
		for (auto& v : m_WMO->m_PortalVertices)
		{
			m_ConvertedVerts.push_back(transformComponent->GetWorldTransfom() * vec4(v, 1.0f));
		}
	}
}

void CWMO_Base_Instance::setWMO(std::shared_ptr<CWMO> _model)
{
	assert1(m_WMO == nullptr);
	assert1(_model != nullptr);

	m_WMO = _model;
}

std::shared_ptr<CWMO> CWMO_Base_Instance::getWMO() const
{
	return m_WMO;
}


//#define WMO_DISABLE_PORTALS

bool CWMO_Base_Instance::Load()
{
	std::shared_ptr<CWMO> wmo = GetManager<IWMOManager>()->Add(m_WMOName);
	if (wmo)
	{
		setWMO(wmo);
		CreateInstances();
		return true;
	}

	return false;
}

bool CWMO_Base_Instance::Delete()
{
	return false;
}



void CWMO_Base_Instance::UpdateCamera(const Camera* camera)
{
#ifndef WMO_DISABLE_PORTALS
	if (m_WMO && m_WMO->m_PortalController)
	{
		m_WMO->m_PortalController->Update(std::dynamic_pointer_cast<CWMO_Base_Instance, SceneNode>(shared_from_this()), *camera);
	}
#endif
}

bool CWMO_Base_Instance::Accept(std::shared_ptr<IVisitor> visitor)
{
	std::shared_ptr<AbstractPass> visitorAsBasePass = std::dynamic_pointer_cast<AbstractPass>(visitor);
 	const Camera* camera = visitorAsBasePass->GetRenderEventArgs()->Camera;

	//if (!GetComponent<CColliderComponent3D>()->CheckDistance2D(camera, GetGroupQuality().ADT_WMO_Distance))
	//{
	//	return false;
	//}

	//if (!GetComponent<CColliderComponent3D>()->CheckFrustum(camera))
	//{
	//	return false;
	//}

	return SceneNode3D::Accept(visitor);
}

const CGroupQuality & CWMO_Base_Instance::GetGroupQuality() const
{
    return m_QualitySettings;
}
