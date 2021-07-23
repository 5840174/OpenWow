#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "MapWDLTileModel.h"

CMapWDLTileModel::CMapWDLTileModel(IRenderDevice& RenderDevice, const CMap& Map, int32 IndexX, int32 IndexZ)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_Map(Map)
	, m_IndexX(IndexX)
	, m_IndexZ(IndexZ)
{}

CMapWDLTileModel::~CMapWDLTileModel()
{}

bool CMapWDLTileModel::IsNeedRender() const
{
	int32 currentX = m_Map.GetCurrentX();
	int32 currentZ = m_Map.GetCurrentZ();

	if (m_Map.IsTileCurrent(m_IndexX, m_IndexZ))
		return false;

	if (glm::abs(m_IndexX - currentX) > 7 || abs(m_IndexZ - currentZ) > 7)
		return false;

	return true;
}
