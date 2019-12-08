#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Group.h"

// General
#include "WMO_Group_Part_Batch.h"

WMO_Group_Part_Batch::WMO_Group_Part_Batch(const std::weak_ptr<const CWMO> _parentWMO, std::shared_ptr<IMesh> _mesh, const SWMO_Group_BatchDef& _proto) 
	: MeshProxie(_mesh)
	, m_ParentWMO(_parentWMO)
	, m_Proto(_proto)
{
	m_WMOMaterial = m_ParentWMO.lock()->m_Materials[m_Proto.material_id];

	m_Bounds.setMin(vec3(_proto.bx, _proto.by, _proto.bz));
	m_Bounds.setMax(vec3(_proto.tx, _proto.ty, _proto.tz));
	m_Bounds.calculateCenter();
}

bool WMO_Group_Part_Batch::Render(const RenderEventArgs* renderEventArgs, const IConstantBuffer* perObject, UINT indexStartLocation, UINT indexCnt, UINT vertexStartLocation, UINT vertexCnt)
{
	SetMaterial(m_WMOMaterial);
	return MeshProxie::Render(renderEventArgs, perObject, m_Proto.indexStart, m_Proto.indexCount/*, m_Proto.vertexStart, m_Proto.vertexEnd - m_Proto.vertexStart*/);
}
