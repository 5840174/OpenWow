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

    // Mesh & textures provider
	virtual void setMeshEnabled(MeshIDType _type, uint32 _value);
	virtual bool isMeshEnabled(uint32 _index) const override;

	// ISceneNode
	void Initialize() override;

private:
	// Mesh provider
	uint32 m_MeshID[(size_t)MeshIDType::Count];
};

#endif