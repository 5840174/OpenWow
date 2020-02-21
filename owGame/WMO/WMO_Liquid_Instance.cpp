#include "stdafx.h"

// General
#include "WMO_Liquid_Instance.h"

CWMO_Liquid_Instance::CWMO_Liquid_Instance()
	: m_PortalVisibilityState(true)
{
}

CWMO_Liquid_Instance::~CWMO_Liquid_Instance()
{
}

void CWMO_Liquid_Instance::Accept(IVisitor* visitor)
{
	if (m_PortalVisibilityState)
	{
		Liquid_Instance::Accept(visitor);
	}
}
