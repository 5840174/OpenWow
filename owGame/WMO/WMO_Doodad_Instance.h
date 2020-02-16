#pragma once

#ifdef USE_M2_MODELS

#include "M2/M2_Base_Instance.h"
#include "WMO_Headers.h"
#include "WMO_Group.h"

class ZN_API CWMO_Doodad_Instance 
	: public CM2_Base_Instance
{
public:
	CWMO_Doodad_Instance(const M2& M2Object, const WMO_Group& WMOGroupObject, uint32 _index, const SWMO_Doodad_PlacementInfo& _placement);
	virtual ~CWMO_Doodad_Instance();

	void setPortalVisibility(bool _visibility) { m_PortalVis = _visibility; }
		
	// SceneNode3D
	void Initialize() override;

	void Accept(IVisitor* visitor) override;

private:
	const WMO_Group&	m_WMOGroupObject;
	const uint32        m_Index;
	bool                m_PortalVis;
};

#endif
