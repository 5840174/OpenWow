#pragma once

#include "Liquid.h"

class ZN_API Liquid_Instance
	: public SceneNode3D
	, public ILiquidInstanceInitializaton
{
public:
	Liquid_Instance();
	virtual ~Liquid_Instance();

	// ILiquidInstanceInitializaton
    void LiquidInitialize(std::shared_ptr<CLiquid> LiquidObject, vec3 Position) override;
};