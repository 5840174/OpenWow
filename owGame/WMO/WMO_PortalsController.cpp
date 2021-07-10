#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include
#include "WMO.h"
#include "WMO_Base_Instance.h"

// General
#include "WMO_PortalsController.h"

// Additional
#include "WMO_Group_Instance.h"

#ifdef USE_WMO_PORTALS_CULLING

CWMO_PortalsController::CWMO_PortalsController()
{}

CWMO_PortalsController::~CWMO_PortalsController()
{}

void CWMO_PortalsController::Update(const CWMO_Base_Instance* WMOBaseInstance, const ICameraComponent3D* _camera)
{
	if (WMOBaseInstance->GetState() != ILoadable::ELoadableState::Loaded)
		return;

	// Reset all flags
	for (const auto& groupPtr : WMOBaseInstance->getGroupInstances())
		if (auto group = groupPtr.lock())
			group->Reset();

	glm::vec3 cameraTranslate = _camera->GetPosition();
	Frustum cameraFrustum = _camera->GetFrustum();
	bool insideIndoor = false;

	BoundingBox wmoBaseInstanceBounds = WMOBaseInstance->GetComponentT<IColliderComponent>()->GetWorldBounds();

	if (wmoBaseInstanceBounds.isPointInside(cameraTranslate))
	{
		for (const auto& groupPtr : WMOBaseInstance->getGroupInstances())
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
		for (const auto& groupPtr : WMOBaseInstance->getGroupInstances())
		{
			if (auto group = groupPtr.lock())
			{
				if (group->IsOutdoor())
				{
					Recur(group, cameraFrustum, cameraTranslate, cameraFrustum, true);
				}
			}
		}
	}
}



//
// Private
//
bool CWMO_PortalsController::Recur(const std::shared_ptr<IPortalRoom>& Room, const Frustum& CameraFrustum, const glm::vec3& CameraPosition, const Frustum& PortalFrustum, bool _isFirstIteration)
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
