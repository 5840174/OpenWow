#include "stdafx.h"

// General
#include "Creature_M2Instance.h"

Creature_M2Instance::Creature_M2Instance(const std::shared_ptr<CM2>& M2Object) 
	: CM2_Base_Instance(M2Object)
{}

Creature_M2Instance::~Creature_M2Instance()
{}



//
// Mesh & textures provider
//
bool Creature_M2Instance::isMeshEnabled(uint32 _index) const
{
	uint32 div100 = _index / 100;
	uint32 mod100 = _index % 100;
	
	return (div100 == 0) && (mod100 == 0);
}