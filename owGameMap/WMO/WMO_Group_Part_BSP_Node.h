#pragma once

#include "WMO_Group_Headers.h"

// FORWARD BEGIN
class WMO_Group;
// FORWARD END

class CWMO_Group_Part_BSP_Node
{
public:
	CWMO_Group_Part_BSP_Node(const std::weak_ptr<const WMO_Group> _parentGroup, const SWMO_Group_MOBNDef& _proto);

	void Render(cmat4 _worldMatrix);

private:
	const SWMO_Group_MOBNDef m_Proto;

	std::shared_ptr<IModel> m_GEOM_Collision;
	vec4 color;
};