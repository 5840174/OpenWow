#pragma once

#include "World/Creature/Creature_M2Instance.h"

#include "World/MeshIDEnums.h"

class ZN_API Creature 
	: public Creature_M2Instance
{
public:
	Creature(const std::shared_ptr<M2>& M2Object);
    virtual ~Creature();

    // Mesh & textures provider
	virtual void setMeshEnabled(MeshIDType::List _type, uint32 _value);
	virtual bool isMeshEnabled(uint32 _index) const override;

private:
	// Mesh provider
	uint32				m_MeshID[MeshIDType::Count];
};