#include "stdafx.h"

// General
#include "WMO_Base_Instance.h"

CWMO_Base_Instance::CWMO_Base_Instance(const CWMO& WMOObject)
    : m_WMOObject(WMOObject)
{
	
}

CWMO_Base_Instance::~CWMO_Base_Instance()
{}




void CWMO_Base_Instance::CreateInstances()
{
	m_WMOObject.CreateInsances(this);

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


const CWMO& CWMO_Base_Instance::getWMO() const
{
	return m_WMOObject;
}



//
// SceneNode
//
void CWMO_Base_Instance::Initialize()
{
	CreateInstances();
}

std::string CWMO_Base_Instance::GetName() const 
{ 
	return "WMO '" + m_WMOObject.getFilename() + "'"; 
}

void CWMO_Base_Instance::UpdateCamera(const ICameraComponent3D* camera)
{
#ifndef WMO_DISABLE_PORTALS
	if (m_WMO && m_WMO->m_PortalController)
	{
		m_WMO->m_PortalController->Update(this, camera);
	}
#endif
}

void CWMO_Base_Instance::Accept(IVisitor* visitor)
{
	SceneNode3D::Accept(visitor);
}
