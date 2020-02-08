#pragma once

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO;
class WMO_Group;
class CWMO_Base_Instance;
// FORWARD END

class CWMO_Part_Portal
{
public:
	CWMO_Part_Portal(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMO_PortalDef& _proto);
	virtual ~CWMO_Part_Portal();

	uint16 getStartVertex() const { return m_StartVertex; }
	uint16 getCount() const { return m_Count; }
	const Plane& getPlane() const { return m_Plane; }

	void setGroup(int32 _group, int16 side);
	int32 getGrInner() { return m_GrInner; }
	int32 getGrOuter() { return m_GrOuter; }

	bool IsVisible(std::shared_ptr<CWMO_Base_Instance> _localContr, const Plane* _planes, uint32 _planesCount) const;
	bool IsPositive(cvec3 _InvWorldCamera) const;

private:
	BoundingBox				m_Bounds;

	uint16					m_StartVertex;
	uint16					m_Count;
	Plane					m_Plane;

	int32					m_GrInner;
	int32					m_GrOuter;

	std::shared_ptr<IGeometry> m_Geom;

private:
	const CWMO& m_WMOModel;
};