#include "stdafx.h"

// General
#include "RenderPass_MapTile.h"

// Additional
#include "Map.h"
#include "MapTile.h"

CRenderPass_MapTile::CRenderPass_MapTile(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene)
	: ScenePass(scene)
{
	m_WoWSettings = RenderDevice.GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_MapTile::~CRenderPass_MapTile()
{}



//
// IVisitor
//
EVisitResult CRenderPass_MapTile::Visit(const ISceneNode* node)
{	
	if (node->Is(cMapTile_NodeType))
	{
		if (const CMapTile* map = static_cast<const CMapTile*>(node))
		{
			if (!map->GetMap().getTileIsCurrent(map->getIndexX(), map->getIndexZ()))
				return EVisitResult::Block;

			ScenePass::Visit(node);
		}
	}

	return EVisitResult::AllowAll;
}
