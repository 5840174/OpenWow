#include "stdafx.h"

// General
#include "WMO_Base_Instance.h"

CWMO_Base_Instance::CWMO_Base_Instance(const std::shared_ptr<CWMO>& WMOObject)
    : CLoadableObject(WMOObject)
	, m_WMOObject(WMOObject)
{
	SetType(cWMO_NodeType);
}

CWMO_Base_Instance::~CWMO_Base_Instance()
{}




void CWMO_Base_Instance::CreateInstances()
{
	m_WMOObject->CreateInsances(std::dynamic_pointer_cast<CWMO_Base_Instance>(shared_from_this()));
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

uint16 CWMO_Base_Instance::GetDoodadSetIndex() const
{
	return 0;
}

bool CWMO_Base_Instance::IsDoodadInSet(uint16 doodadIndex) const
{
	if (getWMO().IsDoodadInSet(0, doodadIndex))
		return true;

	if (GetDoodadSetIndex() != 0)
		return getWMO().IsDoodadInSet(GetDoodadSetIndex(), doodadIndex);

	return false;
}



//
// SceneNode
//
void CWMO_Base_Instance::Initialize()
{
	GetColliderComponent()->SetCullStrategy(IColliderComponent3D::ECullStrategy::ByFrustrumAndDistance);
	GetColliderComponent()->SetBounds(getWMO().GetBounds());
	GetColliderComponent()->SetDebugDrawMode(true);
}

void CWMO_Base_Instance::Update(const UpdateEventArgs& e)
{
	if (GetState() != ILoadable::ELoadableState::Loaded)
		return;

#ifdef USE_WMO_PORTALS_CULLING
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
