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
{
	friend CM2_Skin_Builder;
public:
	CM2_Skin(const std::weak_ptr<const M2> _model);

	void CreateInsances(std::weak_ptr<CM2_Base_Instance> _parent);

	void Draw(CM2_Base_Instance* _instance);

private:
	std::vector<std::shared_ptr<CM2_SkinSection>>	m_Sections;
	std::vector<std::shared_ptr<CM2_Skin_Batch>>   m_Batches;

private: // PARENT
	const std::weak_ptr<const M2> m_ParentM2;
	CGroupQuality& m_QualitySettings;
};