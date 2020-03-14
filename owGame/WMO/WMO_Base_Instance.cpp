#include "stdafx.h"

// General
#include "WMO_Base_Instance.h"

CWMO_Base_Instance::CWMO_Base_Instance(const std::shared_ptr<CWMO>& WMOObject)
    : m_WMOObject(WMOObject)
	, CLoadableObject(WMOObject)
{
	SetType(cWMO_NodeType);
}

CWMO_Base_Instance::~CWMO_Base_Instance()
{}




void CWMO_Base_Instance::CreateInstances()
{
	m_WMOObject->CreateInsances(shared_from_this());
}



//
// CLoadableObject
//
bool CWMO_Base_Instance::Load()
{
	CreateInstances();

	return true;
}


const CWMO& CWMO_Base_Instance::getWMO() const
{
	_ASSERT(m_WMOObject != nullptr);
	return *m_WMOObject;
}



//
// SceneNode
//
void CWMO_Base_Instance::Initialize()
{
	GetColliderComponent()->SetBounds(getWMO().GetBounds());
	GetColliderComponent()->SetDebugDrawMode(false);
}

void CWMO_Base_Instance::Update(const UpdateEventArgs& e)
{
	if (GetState() != ILoadable::ELoadableState::Loaded)
		return;

#ifndef WMO_DISABLE_PORTALS
	if (m_WMOObject->m_PortalController)
	{
		m_WMOObject->m_PortalController->Update(this, e.CameraForCulling);
	}
#endif
}

void CWMO_Base_Instance::Accept(IVisitor* visitor)
{
	SceneNode3D::Accept(visitor);
}
