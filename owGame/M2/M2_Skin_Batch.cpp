#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"
#include "M2_Skin_Builder.h"

// General
#include "M2_Skin_Batch.h"

CM2_Skin_Batch::CM2_Skin_Batch(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const M2& M2Model, const CM2_SkinSection& SkinSection)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
	, m_SkinSection(SkinSection)
{
	
}

CM2_Skin_Batch::~CM2_Skin_Batch()
{
}

bool CM2_Skin_Batch::Render(const RenderEventArgs& renderEventArgs) const
{
	return true;
}

void CM2_Skin_Batch::Accept(IVisitor * visitor)
{
	visitor->Visit(this);

	// Don't visit geometry
}
