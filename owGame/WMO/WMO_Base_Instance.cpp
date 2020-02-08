#include "stdafx.h"

// General
#include "WMO_Base_Instance.h"

CWMO_Base_Instance::CWMO_Base_Instance(std::string _wmoName) 
    : m_WMOName(_wmoName)
{}

CWMO_Base_Instance::~CWMO_Base_Instance()
{}

void CWMO_Base_Instance::Initialize()
{

}


void CWMO_Base_Instance::CreateInstances()
{
	m_WMO->CreateInsances(std::static_pointer_cast<CWMO_Base_Instance>(shared_from_this()));

#ifndef WMO_DISABLE_PORTALS
	if (m_WMO->m_PortalController != nullptr)
	{
		for (auto& v : m_WMO->m_PortalVertices)
		{
			m_ConvertedVerts.push_back(GetWorldTransfom() * vec4(v, 1.0f));
		}
	}
#endif
}

void CWMO_Base_Instance::setWMO(std::shared_ptr<CWMO> _model)
{
	_ASSERT(m_WMO == nullptr);
	_ASSERT(_model != nullptr);

	m_WMO = _model;
}

std::shared_ptr<CWMO> CWMO_Base_Instance::getWMO() const
{
	return m_WMO;
}

bool CWMO_Base_Instance::Load()
{
	std::shared_ptr<CWMO> wmo = GetBaseManager()->GetManager<IWMOManager>()->Add(m_WMOName);
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



void CWMO_Base_Instance::UpdateCamera(const ICameraComponent3D* camera)
{
#ifndef WMO_DISABLE_PORTALS
	if (m_WMO && m_WMO->m_PortalController)
	{
		m_WMO->m_PortalController->Update(std::dynamic_pointer_cast<CWMO_Base_Instance>(shared_from_this()), camera);
	}
#endif
}

bool CWMO_Base_Instance::Accept(IVisitor* visitor)
{
	return SceneNode3D::Accept(visitor);
}
