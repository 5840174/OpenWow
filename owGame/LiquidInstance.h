#pragma once

#include "Liquid.h"

class OW_ENGINE_API Liquid_Instance
	: public CSceneNodeProxie
	, public ILiquidInstanceInitializaton
{
public:
	Liquid_Instance();
	virtual ~Liquid_Instance();

	// ILiquidInstanceInitializaton
    void Initialize(std::shared_ptr<CLiquid> LiquidObject, vec3 Position) override;

	// CSceneNodeProxie
	void Initialize() override;
	void Finalize() override;
};