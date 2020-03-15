#include "stdafx.h"

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

void CWMO_PortalsController::Update(const CWMO_Base_Instance* SceneNodeInstance, const ICameraComponent3D* _camera)
{
	if (SceneNodeInstance->GetState() != ILoadable::ELoadableState::Loaded)
		return;

	// Reset all flags
	for (const auto& groupPtr : SceneNodeInstance->getGroupInstances())
		if (auto group = groupPtr.lock())
			group->Reset();

	glm::vec3 cameraTranslate = _camera->GetTranslation();
	Frustum cameraFrustum = _camera->GetFrustum();
	bool insideIndoor = false;

	BoundingBox wmoBaseInstanceBounds = const_cast<CWMO_Base_Instance*>(SceneNodeInstance)->GetColliderComponent()->GetBounds();
	wmoBaseInstanceBounds.transform(SceneNodeInstance->GetWorldTransfom());

	if (wmoBaseInstanceBounds.isPointInside(cameraTranslate))
	{
		for (const auto& groupPtr : SceneNodeInstance->getGroupInstances())
		{
			auto group = groupPtr.lock();
			if (group == nullptr)
				continue;

			// Group camera culling
			if (group->GetBoundingBox().isPointInside(cameraTranslate) == false)
				continue;

			if (group->getObject().m_GroupHeader.flags.HAS_COLLISION == false)
				continue;

			if (group->getObject().m_GroupHeader.flags.IS_OUTDOOR)
				continue;

			bool recurResult = Recur(SceneNodeInstance, group, cameraFrustum, cameraTranslate, cameraFrustum, true);
			if (!recurResult)
			{
				continue;
			}

			if (group->getObject().m_GroupHeader.flags.IS_INDOOR)
			{
				insideIndoor = true;
			}
		}
	}

	//_ASSERT(insideOneAtLeast || !(m_ParentWMO->m_OutdoorGroups.empty()));

	// If we outside WMO, then get outdorr group
	if (!insideIndoor)
	{
		for (const auto& groupPtr : SceneNodeInstance->getGroupOutdoorInstances())
		{
			if (auto group = groupPtr.lock())
			{
				Recur(SceneNodeInstance, group, cameraFrustum, cameraTranslate, cameraFrustum, true);
			}
			else _ASSERT(false);
		}
	}
}

bool CWMO_PortalsController::Recur(const CWMO_Base_Instance* SceneNodeInstance, const std::shared_ptr<IPortalRoom>& Room, const Frustum& CameraFrustum, const glm::vec3& _InvWorldCamera, const Frustum& PortalFrustum, bool _isFirstIteration)
{
	if (Room == nullptr || Room->IsCalculated())
	{
		return false;
	}

	if (CameraFrustum.cullBox(Room->GetBoundingBox()))
		return false;

	// Set visible for current
	Room->SetVisibilityState(true);
	Room->SetCalculatedState(true);

	for (const auto& roomObjectPtr : Room->GetRoomObjects())
	{
		if (auto roomObject = roomObjectPtr.lock())
		{
			if (_isFirstIteration || PortalFrustum.cullBox(roomObject->GetBoundingBox()) == false)
			{
				roomObject->SetVisibilityState(true);
			}
		}
	}

	for (const auto& p : Room->GetPortals())
	{
		// If we don't see portal // TODO: Don't use it on first step
		if (p->IsVisible(CameraFrustum) == false)
			continue;

		// And we don't see portal from other portal
		if (p->IsVisible(PortalFrustum.getPlanes()) == false)
			continue;

		// Build camera-to-poratl frustum
		Frustum portalFrustum = p->CreatePolyFrustum(_InvWorldCamera);

		// Find attached to portal group
		auto nextRoom = p->GetRoomObject(_InvWorldCamera);

		Recur
		(
			SceneNodeInstance,
			nextRoom,
			CameraFrustum,
			_InvWorldCamera,
			portalFrustum,
			false
		);
	}

	return true;
}

#endif
