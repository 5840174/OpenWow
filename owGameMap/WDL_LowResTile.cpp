#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "WDL_LowResTile.h"

CWDL_LowResTile::CWDL_LowResTile(std::weak_ptr<const CMap> _parent, std::shared_ptr<IMesh> _mesh, uint32 _indexX, uint32 _indexZ) :
	MeshWrapper(_mesh),
	m_MapController(_parent),
	m_IndexX(_indexX),
	m_IndexZ(_indexZ)
{
}


//
// IMesh
//
bool CWDL_LowResTile::Render(const RenderEventArgs* renderEventArgs, const ConstantBuffer* perObject, UINT indexStartLocation, UINT indexCnt, UINT vertexStartLocation, UINT vertexCnt)
{
	std::shared_ptr<const CMap> MapController = m_MapController.lock();
	_ASSERT(MapController != NULL);

	int32 currentX = MapController->GetCurrentX();
	int32 currentZ = MapController->GetCurrentZ();

	if (MapController->getTileIsCurrent(m_IndexX, m_IndexZ))
	{
		return false;
	}

	if (abs(m_IndexX - currentX) > 3 || abs(m_IndexZ - currentZ) > 3)
	{
		return false;
	}

	return base::Render(renderEventArgs, perObject);
}
