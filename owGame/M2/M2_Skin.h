#pragma once

#include "M2_SkinTypes.h"

#include "M2_Skin_Batch.h"
#include "M2_SkinSection.h"

// FORWARD BEGIN
class M2;
class CM2_Base_Instance;
class CM2_Skin_Builder;
// FORWARD END

class CM2_Skin 
	: public ISceneNodeProvider
{
	friend CM2_Skin_Builder;
public:
	CM2_Skin(const M2& M2Model);

	// ISceneNodeProvider
	void CreateInsances(std::weak_ptr<ISceneNode3D> _parent) override;

private:
	std::vector<std::shared_ptr<CM2_SkinSection>>	m_Sections;
	std::vector<std::shared_ptr<CM2_Skin_Batch>>   m_Batches;

private: // PARENT
	const M2& m_M2Model;
};