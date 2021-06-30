#pragma once

#ifdef USE_M2_MODELS

#include "../../M2/M2_Base_Instance.h"
#include "../MeshIDEnums.h"

class ZN_API Creature 
	: public CM2_Base_Instance
{
public:
	Creature(IScene& Scene, const std::shared_ptr<CM2>& M2Object);
    virtual ~Creature();

	// ISceneNode
	void Initialize() override;
};

#endif