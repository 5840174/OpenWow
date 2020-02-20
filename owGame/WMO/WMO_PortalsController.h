#pragma once

#define WMO_DISABLE_PORTALS

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
// FORWARD END

#ifndef WMO_DISABLE_PORTALS

class CWMO_PortalsController
{
public:
	CWMO_PortalsController(const CWMO& WMOObject);
    virtual ~CWMO_PortalsController();
	
	void Update(const CWMO_Base_Instance* SceneNodeInstance, const ICameraComponent3D* _camera);
	bool Recur(const CWMO_Base_Instance* SceneNodeInstance, CWMO_Group_Instance* _group, const ICameraComponent3D* _camera, cvec3 _InvWorldCamera, const Frustum& _frustum, bool _isFirstIteration);


private:
	const CWMO& m_WMOObject;
};

#endif
