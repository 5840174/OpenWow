#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"
#include "M2_Skin_Builder.h"

// General
#include "M2_Skin.h"

// Additional
#include "M2_Part_Material.h"

CM2_Skin::CM2_Skin(IRenderDevice& RenderDevice, const M2& M2Model)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
{}

CM2_Skin::~CM2_Skin()
{
}

void CM2_Skin::Accept(IVisitor * visitor)
{
	visitor->Visit(this);
}
