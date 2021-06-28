#pragma once

#ifdef USE_WMO_MODELS

#include "../Liquid/LiquidBaseInstance.h"

class ZN_API CWMO_Liquid_Instance
	: public CLiquidBaseInstance
	, public IPortalRoomObject
{
public:
	CWMO_Liquid_Instance(IScene& Scene);
	virtual ~CWMO_Liquid_Instance();

	// IPortalRoomObject
	BoundingBox GetBoundingBox() const override final;
	void SetVisibilityState(bool _visibility) override final;

	// SceneNode3D
	void Accept(IVisitor* visitor) override;

private:
	bool m_PortalVisibilityState;
};

#endif
