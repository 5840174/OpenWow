#include "stdafx.h"

// General
#include "WMO_Portal_Instance.h"

CWMOPortalInstance::CWMOPortalInstance(const std::weak_ptr<IPortalRoom>& RoomInner, const std::weak_ptr<IPortalRoom>& RoomOuter, const std::vector<glm::vec3>& PortalVertices, const Plane& PortalPlane)
	: m_RoomInner(RoomInner)
	, m_RoomOuter(RoomOuter)
	, m_Vertices(PortalVertices)
	, m_Plane(PortalPlane)
{
}

CWMOPortalInstance::~CWMOPortalInstance()
{
}



//
// IPortal
//
Frustum CWMOPortalInstance::CreatePolyFrustum(glm::vec3 Eye) const
{
	Plane _portalPlanes[15];
	_ASSERT(m_Vertices.size() < 15);

	for (size_t i = 0; i < m_Vertices.size(); i++)
	{
		glm::vec3 v1 = m_Vertices[i];
		glm::vec3 v2 = m_Vertices[(i + 1) % m_Vertices.size()];

		if (IsPositive(Eye))
		{
			_portalPlanes[i] = Plane(Eye, v1, v2);
		}
		else
		{
			_portalPlanes[i] = Plane(Eye, v2, v1);
		}
	}

	return Frustum(_portalPlanes, m_Vertices.size());
}

bool CWMOPortalInstance::IsVisible(const Frustum& Frustum) const
{
	return Frustum.cullPoly(m_Vertices) == false;
}

bool CWMOPortalInstance::IsVisible(const std::vector<Plane>& Planes) const
{
	if (Planes.empty())
		return false;

	return cullPolyByPlanes(Planes.data(), Planes.size(), m_Vertices) == false;
}

bool CWMOPortalInstance::IsPositive(cvec3 InvTranslateCamera) const
{
	return m_Plane.distToPoint(InvTranslateCamera) > 0.0f;
}

std::shared_ptr<IPortalRoom> CWMOPortalInstance::GetRoomObject(glm::vec3 Eye) const
{
	return IsPositive(Eye) ? m_RoomInner.lock() : m_RoomOuter.lock();
}
