#pragma once

class Creature_M2Instance 
	: public CM2_Base_Instance
{
public:
	Creature_M2Instance();
	virtual ~Creature_M2Instance();

	// Mesh & textures provider
	virtual bool isMeshEnabled(uint32 _index) const override;
};
