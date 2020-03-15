#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Group.h"

// General
#include "WMO_Group_Part_Batch.h"

// Additional

WMO_Group_Part_Batch::WMO_Group_Part_Batch(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMO_Group_BatchDef& WMOGroupBatchProto)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_ParentWMO(WMOModel)
	, m_WMOGroupBatchProto(WMOGroupBatchProto)
{
	m_Bounds.setMin(glm::vec3(m_WMOGroupBatchProto.bx, m_WMOGroupBatchProto.by, m_WMOGroupBatchProto.bz));
	m_Bounds.setMax(glm::vec3(m_WMOGroupBatchProto.tx, m_WMOGroupBatchProto.ty, m_WMOGroupBatchProto.tz));
	m_Bounds.calculateCenter();

	SetBounds(m_Bounds);
}

WMO_Group_Part_Batch::~WMO_Group_Part_Batch()
{
}



//
// ModelProxie
//
bool WMO_Group_Part_Batch::Render(const RenderEventArgs& renderEventArgs) const
{
	return ModelProxie::Render(renderEventArgs);
}
