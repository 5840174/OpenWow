#include "stdafx.h"

// Include
#include "WMO_Base_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

// General
#include "WMO_Group_Instance.h"

CWMO_Group_Instance::CWMO_Group_Instance(const std::shared_ptr<WMO_Group> _object) :
	m_Object(_object),
	m_PortalsVis(true),
	m_Calculated(false)
{
}

CWMO_Group_Instance::~CWMO_Group_Instance()
{
}

void CWMO_Group_Instance::Initialize()
{
    BoundingBox bbox = m_Object->m_Bounds;
    bbox.calculateCenter();
    bbox.transform(GetComponent<CTransformComponent3D>()->GetParentWorldTransform());
    GetComponent<CColliderComponent3D>()->SetBounds(bbox);
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

bool CWMO_Group_Instance::Accept(std::shared_ptr<IVisitor> visitor)
{
	if (!m_PortalsVis)
	{
		return false;
	}

	return SceneNode3D::Accept(visitor);
}
