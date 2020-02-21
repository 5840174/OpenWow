#pragma once

#ifdef USE_M2_MODELS

#include "M2/M2_Base_Instance.h"
#include "WMO_Headers.h"
#include "WMO_Group.h"

class ZN_API CWMO_Doodad_Instance 
	: public CM2_Base_Instance
	, public IPortalRoomObject
{
public:
	CWMO_Doodad_Instance(const M2& M2Object, uint32 _index, const SWMO_Doodad_PlacementInfo& _placement);
	virtual ~CWMO_Doodad_Instance();

	// IPortalRoomObject
	inline void SetVisibilityState(bool _visibility) override { m_PortalVisibilityState = _visibility; }
		
	// SceneNode3D
	void Initialize() override;
	void Accept(IVisitor* visitor) override;

private:
	const uint32        m_Index;
	bool                m_PortalVisibilityState;
};

#endif
