#include "stdafx.h"

// General
#include "WMO_Liquid_Instance.h"

CWMO_Liquid_Instance::CWMO_Liquid_Instance(const std::weak_ptr<const WMO_Group> _group)
    : m_Group(_group)
{
}

CWMO_Liquid_Instance::~CWMO_Liquid_Instance()
{
}

bool CWMO_Liquid_Instance::Accept(std::shared_ptr<IVisitor> visitor)
{
	//if (!m_Group.lock()->m_PortalsVis)
	//{
	//	return false;
	//}

	return Liquid_Instance::Accept(visitor);
}
