#pragma once

#include "M2/M2_Base_Instance.h"

class ZN_API Creature_M2Instance
	: public CM2_Base_Instance
{
public:
	Creature_M2Instance(const std::shared_ptr<CM2>& M2Object);
	virtual ~Creature_M2Instance();

	// Mesh & textures provider
	virtual bool isMeshEnabled(uint32 _index) const override;
};
