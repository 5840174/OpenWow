#pragma once

class CWMOPortalInstance
	: public IPortal
{
public:
	CWMOPortalInstance(const std::weak_ptr<IPortalRoom>& RoomInner, const std::weak_ptr<IPortalRoom>& RoomOuter, const std::vector<glm::vec3>& PortalVertices, const Plane& PortalPlane);
	virtual ~CWMOPortalInstance();

	// IPortal
	Frustum CreatePolyFrustum(glm::vec3 Eye) const override final;
	bool IsVisible(const Frustum& Frustum) const override final;
	bool IsVisible(const std::vector<Plane>& Planes) const override final;
	bool IsPositive(cvec3 InvTranslateCamera) const override final;
	std::shared_ptr<IPortalRoom> GetRoomObject(glm::vec3 Eye) const override final;

private:
	std::weak_ptr<IPortalRoom> m_RoomInner;
	std::weak_ptr<IPortalRoom> m_RoomOuter;
	std::vector<glm::vec3>     m_Vertices;
	Plane				       m_Plane;
};