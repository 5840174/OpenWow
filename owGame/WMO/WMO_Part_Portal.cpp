#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include
#include "WMO.h"

// General
#include "WMO_Part_Portal.h"

CWMO_Part_Portal::CWMO_Part_Portal(IRenderDevice& RenderDevice, const std::vector<glm::vec3>& PortalVertices, const SWMO_PortalDef& _proto)
	: m_GrInner(-1)
	, m_GrOuter(-1)
{
	_ASSERT(_proto.count < 20);

	m_Vertices.assign(PortalVertices.data() + _proto.startVertex, PortalVertices.data() + _proto.startVertex + _proto.count);
	_ASSERT(m_Vertices.size() == _proto.count);

	m_Plane.normal = Fix_XZmY(_proto.plane.normal);
	m_Plane.dist = _proto.plane.distance;
}

CWMO_Part_Portal::~CWMO_Part_Portal()
{
}

void CWMO_Part_Portal::setGroup(int32 _group, int16 side)
{
	if (side == 1)
	{
		m_GrInner = _group;
	}
	else if (side == -1)
	{
		m_GrOuter = _group;
	}
	else _ASSERT(false);
}

#endif
