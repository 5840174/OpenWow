#pragma once

#include "WMO_Headers.h"

// FORWARD BEGIN
class CWMO_Base_Instance;
// FORWARD END

class CWMO_PortalsController
{
public:
	CWMO_PortalsController(const std::weak_ptr<const CWMO> _parentWMO);
    virtual ~CWMO_PortalsController();

	void GetPolyFrustum(const vec3* poly, uint32 num_verts, Frustum* frustum, vec3 eye, bool _isPositive = true);
	
	void Update(std::shared_ptr<CWMO_Base_Instance> _localContr, const ICamera* _camera);
	bool Recur(std::shared_ptr<CWMO_Base_Instance> _localContr, std::shared_ptr<CWMO_Group_Instance> _group, const ICamera* _camera, cvec3 _InvWorldCamera, const Frustum* _frustum, bool _isFirstIteration);


private:
	const std::weak_ptr<const CWMO> m_ParentWMO;
};