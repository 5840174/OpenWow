#pragma once

#ifdef USE_M2_MODELS

#include "../../M2/M2_Base_Instance.h"

class ZN_API GameObjectM2
	: public CM2_Base_Instance
{
public:
	GameObjectM2(IScene& Scene, const std::shared_ptr<CM2>& M2Object);
	virtual ~GameObjectM2();

	// ISceneNode
	void Initialize() override;
};

#endif