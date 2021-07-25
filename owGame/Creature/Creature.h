#pragma once

#ifdef USE_M2_MODELS

#include "../M2/M2_Base_Instance.h"
#include "../M2/MeshIDEnums.h"

class ZN_API CCreature 
	: public CM2_Base_Instance
{
public:
	CCreature(IScene& Scene, CWorldClient& WorldClient, const std::shared_ptr<CM2>& M2Object);
    virtual ~CCreature();

	// ISceneNode
	void Initialize() override;
};

#endif