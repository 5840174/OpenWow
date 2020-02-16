#include "stdafx.h"

// Include
#include "WMO_Base_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

// General
#include "WMO_Group_Instance.h"

CWMO_Group_Instance::CWMO_Group_Instance(const WMO_Group& WMOGroupObject) 
	: m_PortalsVis(true)
	, m_Calculated(false)
	, m_WMOGroupObject(WMOGroupObject)
{
}

CWMO_Group_Instance::~CWMO_Group_Instance()
{
}

void CWMO_Group_Instance::Initialize()
{
    BoundingBox bbox = m_WMOGroupObject.m_Bounds;
    bbox.calculateCenter();
    GetComponent<IColliderComponent3D>()->SetBounds(bbox);
	GetComponent<CColliderComponent3D>()->SetDebugDrawMode(true);
}

void CWMO_Group_Instance::SetPortalVisible(bool Value)
{
    m_PortalsVis = Value;
}

bool CWMO_Group_Instance::GetPortalVisible() const
{
    return m_PortalsVis;
}

void CWMO_Group_Instance::SetPortalCalculated(bool Value)
{
    m_Calculated = Value;
}

bool CWMO_Group_Instance::GetPortalCalculated() const
{
    return m_Calculated;
}

void CWMO_Group_Instance::Accept(IVisitor* visitor)
{
	if (m_PortalsVis)
	{
		SceneNode3D::Accept(visitor);
	}	
}
