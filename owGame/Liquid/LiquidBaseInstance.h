#pragma once

#include "Liquid.h"

class ZN_API CLiquidBaseInstance
	: public CSceneNode
{
public:
	CLiquidBaseInstance(IScene& Scene);
	virtual ~CLiquidBaseInstance();

	// ISceneNode
	void Initialize() override;
};