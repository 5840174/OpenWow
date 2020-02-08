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
	CM2_Skin_Builder(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const CM2_Builder& _m2Builder, const M2& M2Model, const SM2_SkinProfile& SkinProfileProto, std::shared_ptr<IFile> _file);
	virtual ~CM2_Skin_Builder();

	std::shared_ptr<CM2_Skin> Load();

	// Loader
	void Step1LoadProfile(const std::shared_ptr<CM2_Skin>& M2SkinObject);
	void Step2InitBatches(const std::shared_ptr<CM2_Skin>& M2SkinObject);

private:
	std::weak_ptr<IFile>		m_F;

private:
	IBaseManager*       m_BaseManager;
	IRenderDevice&      m_RenderDevice;
	const CM2_Builder&  m_M2Builder;
	const M2&			m_M2Model;
	const SM2_SkinProfile& m_SkinProfileProto;
};