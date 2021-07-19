#pragma once

#ifdef USE_WMO_MODELS

#include "WMOGroup_Headers.h"

// FORWARD BEGIN
class CWMOGroup;
// FORWARD END

class CWMOGroup_Part_CollisionNode
{
public:
	CWMOGroup_Part_CollisionNode(IRenderDevice& RenderDevice, const CWMOGroup& WMOGroup, const SWMOGroup_MOBN& Proto, const std::vector<glm::vec3>& VerticesArray, const std::vector<uint16>& CollisionIndicesArray);
	virtual ~CWMOGroup_Part_CollisionNode();

	const SWMOGroup_MOBN& GetProto() const;
	const std::shared_ptr<IGeometry>& GetCollisionGeometry() const;

private:
	std::shared_ptr<IGeometry> m_CollisionGeom;

private:
	const SWMOGroup_MOBN m_Proto;
};

#endif
