#include "stdafx.h"

// General
#include "RenderPass_MapTile.h"

// Additional
#include "Map.h"
#include "MapTile.h"

CRenderPass_MapTile::CRenderPass_MapTile(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene)
	: ScenePass(RenderDevice, scene)
{
	m_WoWSettings = RenderDevice->GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_MapTile::~CRenderPass_MapTile()
{}



//
// IVisitor
//
bool CRenderPass_MapTile::Visit(const ISceneNode3D* node)
{
	const CMapTile* map = dynamic_cast<const CMapTile*>(node);
    if (map == nullptr)
        return false;

	if (!map->GetMapController()->getTileIsCurrent(map->m_IndexX, map->m_IndexZ))
		return false;

    return ScenePass::Visit(node);
}
