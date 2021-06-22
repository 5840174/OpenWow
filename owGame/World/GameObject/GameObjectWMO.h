#pragma once

#ifdef USE_WMO_MODELS

#include "WMO/WMO_Base_Instance.h"

class ZN_API GameObjectWMO
	: public CWMO_Base_Instance
{
public:
	GameObjectWMO(IScene& Scene, const std::shared_ptr<CWMO>& M2Object);
	virtual ~GameObjectWMO();

	// ISceneNode
	void Initialize() override;
};

#endif