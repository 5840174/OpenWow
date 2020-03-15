#pragma once

#include "WMO_Group_Headers.h"

// FORWARD BEGIN
class WMO_Group;
// FORWARD END

class CWMO_Group_Part_BSP_Node
{
public:
	CWMO_Group_Part_BSP_Node(const WMO_Group& WMOGroup, const SWMO_Group_MOBNDef& _proto);
	virtual ~CWMO_Group_Part_BSP_Node();

	void Render(const glm::mat4& _worldMatrix);

private:
	const SWMO_Group_MOBNDef m_Proto;

	std::shared_ptr<IModel> m_GEOM_Collision;
	glm::vec4 color;
};