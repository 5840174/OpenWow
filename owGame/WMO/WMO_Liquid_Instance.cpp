#include "stdafx.h"

// General
#include "WMO_Liquid_Instance.h"

CWMO_Liquid_Instance::CWMO_Liquid_Instance(const WMO_Group& WMOGroupObject)
    : m_WMOGroupObject(WMOGroupObject)
{
}

CWMO_Liquid_Instance::~CWMO_Liquid_Instance()
{
}

void CWMO_Liquid_Instance::Accept(IVisitor* visitor)
{
	//if (m_Group.lock()->m_PortalsVis)
	//{
		Liquid_Instance::Accept(visitor);
	//}
}
