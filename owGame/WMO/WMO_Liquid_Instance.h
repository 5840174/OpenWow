#pragma once

#ifdef USE_WMO_MODELS

#include "Liquid/LiquidInstance.h"
#include "WMO_Headers.h"
#include "WMO_Group.h"

class ZN_API CWMO_Liquid_Instance
	: public Liquid_Instance
	, public IPortalRoomObject
{
public:
	CWMO_Liquid_Instance();
	virtual ~CWMO_Liquid_Instance();

	// IPortalRoomObject
	BoundingBox GetBoundingBox() const override final;
	inline void SetVisibilityState(bool _visibility) override { m_PortalVisibilityState = _visibility; }

	// SceneNode3D
	void Accept(IVisitor* visitor) override;

private:
	bool m_PortalVisibilityState;
};

#endif
