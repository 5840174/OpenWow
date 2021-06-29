#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "WMO_Liquid_Instance.h"

CWMO_Liquid_Instance::CWMO_Liquid_Instance(IScene& Scene)
	: CLiquidBaseInstance(Scene)
	, m_PortalVisibilityState(true)
{}

CWMO_Liquid_Instance::~CWMO_Liquid_Instance()
{}



//
// IPortalRoomObject
//
BoundingBox CWMO_Liquid_Instance::GetBoundingBox() const
{
	return GetComponentT<IColliderComponent>()->GetWorldBounds();
}

void CWMO_Liquid_Instance::SetVisibilityState(bool _visibility)
{
	m_PortalVisibilityState = _visibility;
}



//
// ISceneNode
//
void CWMO_Liquid_Instance::Accept(IVisitor* visitor)
{
	if (m_PortalVisibilityState)
		__super::Accept(visitor);
}

#endif
