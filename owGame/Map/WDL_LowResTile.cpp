#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "WDL_LowResTile.h"

CWDL_LowResTile::CWDL_LowResTile(IRenderDevice& RenderDevice, const CMap& Map, int IndexX, int IndexZ)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_MapController(Map)
	, m_IndexX(IndexX)
	, m_IndexZ(IndexZ)
{
}

CWDL_LowResTile::~CWDL_LowResTile()
{
}

bool CWDL_LowResTile::IsNeedRender() const
{
	int32 currentX = m_MapController.GetCurrentX();
	int32 currentZ = m_MapController.GetCurrentZ();

	if (m_MapController.getTileIsCurrent(m_IndexX, m_IndexZ))
	{
		return false;
	}

	if (glm::abs(m_IndexX - currentX) > 7 || abs(m_IndexZ - currentZ) > 7)
	{
		return false;
	}

	return true;
}
