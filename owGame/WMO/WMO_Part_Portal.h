#pragma once

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO;
// FORWARD END

class CWMO_Part_Portal
{
public:
	CWMO_Part_Portal(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMO_PortalDef& Proto);
	virtual ~CWMO_Part_Portal();

	const std::vector<glm::vec3>& GetVertices() const { return m_Vertices; }
	const Plane& getPlane() const { return m_Plane; }

	void setGroup(int32 _group, int16 side);
	int32 getGrInner() const { return m_GrInner; }
	int32 getGrOuter() const { return m_GrOuter; }

private:
	uint16					m_PortalVertexesOffset;
	uint16					m_PortalVertexesCount;
	std::vector<glm::vec3>  m_Vertices;
	Plane					m_Plane;

	int32					m_GrInner;
	int32					m_GrOuter;
};