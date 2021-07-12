#pragma once

#ifdef USE_WMO_MODELS
#ifdef USE_WMO_PORTALS_CULLING

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
// FORWARD END

class ZN_API CWMO_PortalsComponent
	: public CSceneNodeComponentBase
{
public:
	ZN_OBJECTCLASS(cSceneNodeComponentWMOPortalsComponent);

	CWMO_PortalsComponent(const CWMO_Base_Instance& OwnerNode);
    virtual ~CWMO_PortalsComponent();
	
	// ISceneNodeComponent
	void Update(const UpdateEventArgs& e) override;

protected:
	const CWMO_Base_Instance& GetWMOOwnerNode() const;

private:
	bool Recur(const std::shared_ptr<IPortalRoom>& Room, const Frustum& CameraFrustum, const glm::vec3& CameraPosition, const Frustum& _frustum, bool _isFirstIteration);
};

#endif
#endif
