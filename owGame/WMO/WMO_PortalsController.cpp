#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Base_Instance.h"

// General
#include "WMO_PortalsController.h"

// Additional
#include "WMO_Group_Instance.h"

#ifndef WMO_DISABLE_PORTALS

CWMO_PortalsController::CWMO_PortalsController(const CWMO& WMOObject) 
	: m_WMOObject(WMOObject)
{}

CWMO_PortalsController::~CWMO_PortalsController()
{}

void CWMO_PortalsController::Update(const CWMO_Base_Instance* SceneNodeInstance, const ICameraComponent3D* _camera)
{
	if (SceneNodeInstance->GetState() != ILoadable::ELoadableState::Loaded)
		return;

	// Reset all flags
	for (const auto& groupPtr : SceneNodeInstance->getGroupInstances())
	{
		if (auto group = groupPtr.lock())
		{
			group->SetVisibilityState(false);
			group->SetCalculatedState(false);

			for (const auto& roomObjectPtr : group->GetRoomObjects())
				if (auto roomObject = roomObjectPtr.lock())
					roomObject->SetVisibilityState(false);
		}
	}

	glm::vec3 InvCameraTranslate = SceneNodeInstance->GetInverseWorldTransform() * glm::vec4(_camera->GetTranslation(), 1.0f);
	glm::vec3 InvCameraDirection = SceneNodeInstance->GetInverseWorldTransform() * glm::vec4(_camera->GetDirection(), 0.0f);
	glm::vec3 InvCameraUpDirection = SceneNodeInstance->GetInverseWorldTransform() * glm::vec4(_camera->GetCameraUpDirection(), 0.0f);

	glm::mat4 IvnCameraViewMatrix = glm::lookAt(
		InvCameraTranslate, 
		InvCameraTranslate + InvCameraDirection, 
		InvCameraUpDirection
	);

	Frustum cameraFrustum;
	cameraFrustum.buildViewFrustum(IvnCameraViewMatrix, _camera->GetProjectionMatrix());



	bool insideIndoor = false;

	if (m_WMOObject.GetBounds().isPointInside(InvCameraTranslate))
	{
		for (const auto& groupPtr : SceneNodeInstance->getGroupInstances())
		{
			if (auto group = groupPtr.lock())
			{
				BoundingBox bbox = group->GetComponent<CColliderComponent3D>()->GetBounds();
				bbox.transform(group->GetLocalTransform());

				if (bbox.isPointInside(InvCameraTranslate) == false)
					continue;

				if (group->getObject().m_GroupHeader.flags.HAS_COLLISION == false)
					continue;

				if (group->getObject().m_GroupHeader.flags.IS_OUTDOOR)
				{
					continue;
				}

				bool recurResult = Recur(SceneNodeInstance, group, cameraFrustum, InvCameraTranslate, cameraFrustum, true);
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
	}

	//_ASSERT(insideOneAtLeast || !(m_ParentWMO->m_OutdoorGroups.empty()));

	// If we outside WMO, then get outdorr group
	if (!insideIndoor)
	{
		for (const auto& groupPtr : SceneNodeInstance->getGroupOutdoorInstances())
		{
			if (auto group = groupPtr.lock())
			{
				Recur(SceneNodeInstance, group, cameraFrustum, InvCameraTranslate, cameraFrustum, true);
			}
			else _ASSERT(false);
		}
	}
}

bool CWMO_PortalsController::Recur(const CWMO_Base_Instance* SceneNodeInstance, const std::shared_ptr<IPortalRoom>& Room, const Frustum& CameraFrustum, cvec3 _InvWorldCamera, const Frustum& PortalFrustum, bool _isFirstIteration)
{
	if (Room == nullptr || Room->IsCalculated())
	{
		return false;
	}

	auto roomAsSceneNode = std::dynamic_pointer_cast<ISceneNode3D>(Room);
	_ASSERT(roomAsSceneNode != nullptr);

	BoundingBox bbox = roomAsSceneNode->GetComponent<CColliderComponent3D>()->GetBounds();
	bbox.transform(roomAsSceneNode->GetLocalTransform());
	if (CameraFrustum.cullBox(bbox))
	{
		//return false;
	}

	// Set visible for current
	Room->SetVisibilityState(true);
	Room->SetCalculatedState(true);

	for (const auto& roomObjectPtr : Room->GetRoomObjects())
	{
		if (auto roomObject = roomObjectPtr.lock())
		{
			auto roomObjectAsSceneNode = std::dynamic_pointer_cast<ISceneNode3D>(roomObject);
			_ASSERT(roomObjectAsSceneNode != nullptr);

			BoundingBox bbox = roomObjectAsSceneNode->GetComponent<IColliderComponent3D>()->GetBounds();
			bbox.transform(roomObjectAsSceneNode->GetLocalTransform());

			if (_isFirstIteration || !PortalFrustum.cullBox(bbox))
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
