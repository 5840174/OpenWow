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



//
// IModel
//
bool CWDL_LowResTile::Render(const RenderEventArgs& renderEventArgs) const
{
	int32 currentX = m_MapController.GetCurrentX();
	int32 currentZ = m_MapController.GetCurrentZ();

	if (m_MapController.getTileIsCurrent(m_IndexX, m_IndexZ))
	{
		return false;
	}

	if (abs(m_IndexX - currentX) > 3 || abs(m_IndexZ - currentZ) > 3)
	{
		return false;
	}

	return ModelProxie::Render(renderEventArgs);
}

