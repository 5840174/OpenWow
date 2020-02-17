#pragma once

#include "Creature_M2Instance.h"

#include "MeshIDEnums.h"

class Creature : public Creature_M2Instance
{
public:
	Creature();
    virtual ~Creature();

	// Initialization
	virtual void InitFromDisplayInfo(uint32 _id);

    // Mesh & textures provider
	virtual void setMeshEnabled(MeshIDType::List _type, uint32 _value);
	virtual bool isMeshEnabled(uint32 _index) const override;

protected:
	void CreateCreatureModel(const DBC_CreatureDisplayInfoRecord* CreatureDisplayInfo);

private:
	// Mesh provider
	uint32				m_MeshID[MeshIDType::Count];
};