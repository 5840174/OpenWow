#pragma once

#ifdef USE_WMO_MODELS

#ifdef USE_M2_MODELS

#include "../M2/M2_Base_Instance.h"

#include "WMO_Headers.h"

class ZN_API CWMO_Doodad_Instance 
	: public CM2_Base_Instance
	, public IPortalRoomObject
{
public:
	CWMO_Doodad_Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object, uint32 _index, const SWMO_MODD& _placement);
	virtual ~CWMO_Doodad_Instance();

	// IPortalRoomObject
	BoundingBox GetBoundingBox() const override final;
	inline void SetVisibilityState(bool _visibility) override { m_PortalVisibilityState = _visibility; }
		
	// ISceneNode
	void Initialize() override;
	void Accept(IVisitor* visitor) override;

private:
	const uint32        m_Index;
	const SWMO_MODD& m_Placement;

	bool                m_PortalVisibilityState;
};

#endif

#endif
