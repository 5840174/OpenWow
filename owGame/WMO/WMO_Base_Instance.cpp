#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "WMO_Base_Instance.h"

CWMO_Base_Instance::CWMO_Base_Instance(IScene& Scene, const std::shared_ptr<CWMO>& WMOObject)
    : CSceneNode(Scene)
	, CLoadableObject(WMOObject)
	, m_WMOObject(WMOObject)
{
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

bool CWMO_Base_Instance::Delete()
{
	return false;
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
	__super::Initialize();

	if (auto colliderComponent = GetComponentT<IColliderComponent>())
	{
		colliderComponent->SetCullStrategy(IColliderComponent::ECullStrategy::ByFrustrumAndDistance);
		colliderComponent->SetCullDistance(GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("ADT_WMO_Distance")->Get());
		colliderComponent->SetBounds(getWMO().GetBounds());
		colliderComponent->SetDebugDrawMode(false);
		colliderComponent->SetDebugDrawColor(ColorRGBA(0.8f, 0.8f, 0.2f, 0.8f));
	}
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
	__super::Accept(visitor);
}

#endif
