#include "stdafx.h"

// General
#include "RenderPass_MapTile.h"

// Additional
#include "Map.h"
#include "MapTile.h"

CRenderPass_MapTile::CRenderPass_MapTile(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene)
	: ScenePass(RenderDevice, scene)
{
	m_WoWSettings = RenderDevice.GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_MapTile::~CRenderPass_MapTile()
{}



//
// IVisitor
//
bool CRenderPass_MapTile::Visit(const ISceneNode3D* node)
{	
	if (const CMapTile* map = dynamic_cast<const CMapTile*>(node))
	{
		if (!map->GetMap().getTileIsCurrent(map->getIndexX(), map->getIndexZ()))
			return false;

		ScenePass::Visit(node);
	}

	return false;
}
