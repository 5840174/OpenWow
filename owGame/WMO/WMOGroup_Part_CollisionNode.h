#pragma once

#ifdef USE_WMO_MODELS

#include "WMOGroup_Headers.h"

// FORWARD BEGIN
class CWMOGroup;
// FORWARD END

class CWMOGroup_Part_CollisionNode
{
public:
	CWMOGroup_Part_CollisionNode(const CWMOGroup& WMOGroup, const SWMOGroup_MOBN& _proto);
	virtual ~CWMOGroup_Part_CollisionNode();

	void Render(const glm::mat4& _worldMatrix);

private:
	const SWMOGroup_MOBN m_Proto;

	std::shared_ptr<IModel> m_GEOM_Collision;
	glm::vec4 color;
};

#endif
