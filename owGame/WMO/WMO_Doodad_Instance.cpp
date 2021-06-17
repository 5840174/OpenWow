#include "stdafx.h"

#ifdef USE_WMO_MODELS

#ifdef USE_M2_MODELS

// General
#include "WMO_Doodad_Instance.h"

// Additional
#include "WMO_Base_Instance.h"

CWMO_Doodad_Instance::CWMO_Doodad_Instance(const std::shared_ptr<CM2>& M2Object, uint32 _index, const SWMO_Doodad_PlacementInfo & _placement)
	: CM2_Base_Instance(M2Object)
	, m_Index(_index)
	, m_PortalVisibilityState(true)
{
	// CTransformComponent
	{
		SetPosition(Fix_XZmY(_placement.position));
		SetRotationQuaternion(glm::quat(_placement.orientation.w, -_placement.orientation.z, _placement.orientation.x, _placement.orientation.y));
		SetScale(glm::vec3(_placement.scale, -_placement.scale, -_placement.scale));
	}
}

CWMO_Doodad_Instance::~CWMO_Doodad_Instance()
{}



//
// IPortalRoomObject
//
BoundingBox CWMO_Doodad_Instance::GetBoundingBox() const
{
	return GetComponentT<IColliderComponent>()->GetWorldBounds();
}



//
// SceneNode3D
//
void CWMO_Doodad_Instance::Initialize()
{
	__super::Initialize();
}

void CWMO_Doodad_Instance::Accept(IVisitor* visitor)
{
	auto parentOfParent = std::dynamic_pointer_cast<CWMO_Base_Instance>(GetParent().lock()->GetParent().lock());

	if (parentOfParent->IsDoodadInSet(m_Index))
	{
		if (m_PortalVisibilityState)
		{
			return CM2_Base_Instance::Accept(visitor);
		}
	}
}

#endif

#endif
