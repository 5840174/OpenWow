#pragma once

#ifdef USE_WMO_MODELS

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
// FORWARD END

#ifdef USE_WMO_PORTALS_CULLING

class CWMO_PortalsController
{
public:
	CWMO_PortalsController();
    virtual ~CWMO_PortalsController();
	
	void Update(const CWMO_Base_Instance* WMOBaseInstance, const ICameraComponent3D* _camera);

private:
	bool Recur(const std::shared_ptr<IPortalRoom>& Room, const Frustum& CameraFrustum, const glm::vec3& CameraPosition, const Frustum& _frustum, bool _isFirstIteration);
};

#endif

#endif
