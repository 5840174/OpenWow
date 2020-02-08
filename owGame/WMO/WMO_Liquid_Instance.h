#pragma once

#include "LiquidInstance.h"
#include "WMO_Headers.h"
#include "WMO_Group.h"

class ZN_API CWMO_Liquid_Instance
	: public Liquid_Instance
{
public:
	CWMO_Liquid_Instance(const WMO_Group& WMOGroupObject);
	virtual ~CWMO_Liquid_Instance();

	// SceneNode3D
	void Accept(IVisitor* visitor) override;

private:
	const WMO_Group& m_WMOGroupObject; // used for visibility
};