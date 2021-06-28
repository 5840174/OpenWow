#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include
#include "WMO.h"
#include "WMOGroup.h"

// General
#include "WMOGroup_Part_Batch.h"

// Additional

WMO_Group_Part_Batch::WMO_Group_Part_Batch(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMOGroup_MOBA& WMOGroupBatchProto)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_ParentWMO(WMOModel)
	, m_BatchType(EBatchType::BatchType_Trans)
{
	m_Bounds.setMin(glm::vec3(WMOGroupBatchProto.bx, WMOGroupBatchProto.by, WMOGroupBatchProto.bz));
	m_Bounds.setMax(glm::vec3(WMOGroupBatchProto.tx, WMOGroupBatchProto.ty, WMOGroupBatchProto.tz));
}

WMO_Group_Part_Batch::~WMO_Group_Part_Batch()
{
}



//
// ModelProxie
//
bool WMO_Group_Part_Batch::Render(const ShaderMap& Shaders) const
{
	return ModelProxie::Render(Shaders);
}

void WMO_Group_Part_Batch::Accept(IVisitor * visitor)
{
	//if (GetBatchType() == EBatchType::BatchType_Int)
		return __super::Accept(visitor);
}

#endif
