#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "WMO_Base_Instance.h"

CWMO_Base_Instance::CWMO_Base_Instance(IScene& Scene, const std::shared_ptr<CWMO>& WMOObject)
    : CSceneNode(Scene)
	, CLoadableObject(WMOObject)
	, m_WMOObject(WMOObject)
{
	SetUpdateEnabled(false);
}

CWMO_Base_Instance::~CWMO_Base_Instance()
{
	GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_WMOObject);
}






const CWMO& CWMO_Base_Instance::GetWMO() const
{
	if (m_WMOObject->GetState() != ILoadable::ELoadableState::Loaded)
		throw CException("CWMO_Base_Instance::GetWMO: WMO object isn't loaded.");
	return *m_WMOObject;
}

uint16 CWMO_Base_Instance::GetDoodadSetIndex() const
{
	return 0;
}

bool CWMO_Base_Instance::IsDoodadInSet(uint16 doodadIndex) const
{
	if (GetWMO().IsDoodadInSet(0, doodadIndex))
		return true;

	if (GetDoodadSetIndex() != 0)
		return GetWMO().IsDoodadInSet(GetDoodadSetIndex(), doodadIndex);

	return false;
}



//
// ISceneNode
//
void CWMO_Base_Instance::Initialize()
{
	__super::Initialize();

	if (auto colliderComponent = GetComponentT<IColliderComponent>())
	{
		colliderComponent->SetCullStrategy(IColliderComponent::ECullStrategy::ByFrustrumAndDistance2D);
		colliderComponent->SetCullDistance(GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("WMOBaseRenderDistance")->Get());
		colliderComponent->SetDebugDrawMode(false);
		colliderComponent->SetDebugDrawColor(ColorRGBA(0.8f, 0.8f, 0.2f, 0.8f));
	}
}

void CWMO_Base_Instance::Update(const UpdateEventArgs& e)
{
	if (GetState() != ILoadable::ELoadableState::Loaded)
		return;

#ifdef USE_WMO_PORTALS_CULLING
	if (auto portalsController = GetWMO().GetPortalController())
	{
		portalsController->Update(this, e.CameraForCulling);
	}
#endif
}

void CWMO_Base_Instance::Accept(IVisitor* visitor)
{
	__super::Accept(visitor);
}



//
// ILoadable
//
bool CWMO_Base_Instance::Load()
{
	if (auto colliderComponent = GetComponentT<IColliderComponent>())
	{
		colliderComponent->SetBounds(GetWMO().GetBounds());
	}

	GetWMO().CreateInsances(std::dynamic_pointer_cast<CWMO_Base_Instance>(shared_from_this()));

	return true;
}

bool CWMO_Base_Instance::Delete()
{
	return false;
}

#endif
