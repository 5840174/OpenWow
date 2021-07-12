#include "stdafx.h"

#ifdef USE_WMO_MODELS
#ifdef USE_WMO_PORTALS_CULLING

// Include
#include "WMO.h"
#include "WMO_Base_Instance.h"

// General
#include "WMO_PortalsComponent.h"

// Additional
#include "WMO_Group_Instance.h"

CWMO_PortalsComponent::CWMO_PortalsComponent(const CWMO_Base_Instance& OwnerNode)
	: CSceneNodeComponentBase(OwnerNode)
{}

CWMO_PortalsComponent::~CWMO_PortalsComponent()
{}



//
// ISceneNodeComponent
//
void CWMO_PortalsComponent::Update(const UpdateEventArgs& e)
{
	if (false == GetWMOOwnerNode().IsLoaded())
		return;

	// Reset all flags
	for (const auto& groupPtr : GetWMOOwnerNode().getGroupInstances())
		if (auto group = groupPtr.lock())
			group->Reset();

	const ICameraComponent3D * camera = e.CameraForCulling;
	glm::vec3 cameraTranslate = camera->GetPosition();
	Frustum cameraFrustum = camera->GetFrustum();
	bool insideIndoor = false;

	BoundingBox wmoBaseInstanceBounds = GetWMOOwnerNode().GetComponentT<IColliderComponent>()->GetWorldBounds();

	if (wmoBaseInstanceBounds.isPointInside(cameraTranslate))
	{
		for (const auto& groupPtr : GetWMOOwnerNode().getGroupInstances())
		{
			std::shared_ptr<IPortalRoom> portalRoom = groupPtr.lock();
			if (portalRoom == nullptr)
				continue;

			if (false == portalRoom->IsPointInside(cameraTranslate))
				continue;

			// Group camera culling
			//if (false == portalRoom->GetBoundingBox().isPointInside(cameraTranslate) || false == portalRoom->IsPointInside(cameraTranslate))
			//	continue;

			//if (false == group->GetWMOGroup().m_GroupHeader.flags.HAS_COLLISION)
			//	continue;

			//if (portalRoom->IsOutdoor())
			//	continue;

			if (false == Recur(portalRoom, cameraFrustum, cameraTranslate, cameraFrustum, true))
				continue;

			if (portalRoom->IsIndoor())
			{
				insideIndoor = true;
			}
		}
	}

	//_ASSERT(insideOneAtLeast || !(m_ParentWMO->m_OutdoorGroups.empty()));

	// If we outside WMO, then get outdorr group
	//if (false == insideIndoor)
	{
		for (const auto& groupPtr : GetWMOOwnerNode().getGroupInstances())
		{
			if (auto group = groupPtr.lock())
			{
				if (group->IsLoaded() && group->IsOutdoor())
				{
					Recur(group, cameraFrustum, cameraTranslate, cameraFrustum, true);
				}
			}
		}
	}
}



//
// Protected
//
const CWMO_Base_Instance& CWMO_PortalsComponent::GetWMOOwnerNode() const
{
	return dynamic_cast<const CWMO_Base_Instance&>(GetOwnerNode());
}



//
// Private
//
bool CWMO_PortalsComponent::Recur(const std::shared_ptr<IPortalRoom>& Room, const Frustum& CameraFrustum, const glm::vec3& CameraPosition, const Frustum& PortalFrustum, bool _isFirstIteration)
{
	if (Room->IsCalculated())
		return false;

	if (CameraFrustum.cullBox(Room->GetBoundingBox()))
		return false;

	// Set visible for current
	Room->SetVisibilityState(true);
	Room->SetCalculatedState(true);

	for (const auto& roomObjectPtr : Room->GetRoomObjects())
	{
		if (auto roomObject = roomObjectPtr.lock())
		{
			if (_isFirstIteration || (false == PortalFrustum.cullBox(roomObject->GetBoundingBox())))
			{
				roomObject->SetVisibilityState(true);
			}
		}
	}

	for (const auto& p : Room->GetPortals())
	{
		// If we don't see portal // TODO: Don't use it on first step
		if (false == p->IsVisible(CameraFrustum))
			continue;

		// And we don't see portal from other portal
		if (false == p->IsVisible(PortalFrustum.getPlanes()))
			continue;

		// Build camera-to-poratl frustum
		Frustum portalFrustum = p->CreatePolyFrustum(CameraPosition);

		// Find attached to portal group
		auto nextRoom = p->GetRoomObject(CameraPosition);

		Recur
		(
			nextRoom,
			CameraFrustum,
			CameraPosition,
			portalFrustum,
			false
		);
	}

	return true;
}

#endif

#endif
