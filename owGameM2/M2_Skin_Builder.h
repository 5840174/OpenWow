#pragma once

#include "M2.h"
#include "M2_Skin.h"
#include "M2_SkinSection.h"

// FORWARD BEGIN
class CM2_Builder;
// FORWARD END

class CM2_Skin_Builder
{
public:
	CM2_Skin_Builder(IBaseManager* BaseManager, const CM2_Builder& _m2Builder, std::weak_ptr<M2> _model, const SM2_SkinProfile& _skinProto, std::shared_ptr<CM2_Skin> _skin, std::shared_ptr<IFile> _file);

	void Load();

	// Loader
	void Step1LoadProfile();
	void Step2InitBatches();

private:
	SM2_SkinProfile				m_SkinProfile;
	std::weak_ptr<CM2_Skin>		m_Skin;
	std::weak_ptr<IFile>		m_F;

	//

	std::vector<SM2_SkinBatch>	m_SkinBatches;

	//--
	const CM2_Builder&  m_M2Builder;
	std::weak_ptr<M2>			m_ParentM2;

private:
	IBaseManager*           m_BaseManager;
};