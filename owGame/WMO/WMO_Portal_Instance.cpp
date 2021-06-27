#include "stdafx.h"

#ifdef USE_WMO_MODELS

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
// CWMOPortalInstance
//
const std::vector<glm::vec3>& CWMOPortalInstance::GetVertices() const
{
	return m_Vertices;
}



//
// IPortal
//
Frustum CWMOPortalInstance::CreatePolyFrustum(glm::vec3 CameraPosition) const
{
	Plane _portalPlanes[15];
	_ASSERT(m_Vertices.size() < 15);

	for (size_t i = 0; i < m_Vertices.size(); i++)
	{
		glm::vec3 v1 = m_Vertices[i];
		glm::vec3 v2 = m_Vertices[(i + 1) % m_Vertices.size()];

		if (IsPositive(CameraPosition))
		{
			_portalPlanes[i] = Plane(CameraPosition, v1, v2);
		}
		else
		{
			_portalPlanes[i] = Plane(CameraPosition, v2, v1);
		}
	}

	return Frustum(_portalPlanes, m_Vertices.size());
}

bool CWMOPortalInstance::IsVisible(const Frustum& Frustum) const
{
	return false == Frustum.cullPoly(m_Vertices);
}

bool CWMOPortalInstance::IsVisible(const std::vector<Plane>& Planes) const
{
	return false == cullPolyByPlanes(Planes.data(), Planes.size(), m_Vertices);
}

bool CWMOPortalInstance::IsPositive(const glm::vec3& CameraPosition) const
{
	return m_Plane.distToPoint(CameraPosition) > 0.0f;
}

std::shared_ptr<IPortalRoom> CWMOPortalInstance::GetInnerRoom() const
{
	return m_RoomInner.lock();
}

std::shared_ptr<IPortalRoom> CWMOPortalInstance::GetOuterRoom() const
{
	return m_RoomOuter.lock();
}

std::shared_ptr<IPortalRoom> CWMOPortalInstance::GetRoomObject(glm::vec3 CameraPosition) const
{
	return IsPositive(CameraPosition) ? m_RoomInner.lock() : m_RoomOuter.lock();
}

#endif