#include "stdafx.h"

// General
#include "RenderPass_MapTile.h"

// Additional
#include "Map/Map.h"
#include "Map/MapTile.h"

CRenderPass_MapTile::CRenderPass_MapTile(IScene& Scene)
	: Base3DPass(Scene)
{
	m_WoWSettings = GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_MapTile::~CRenderPass_MapTile()
{}



//
// IVisitor
//
EVisitResult CRenderPass_MapTile::Visit(const std::shared_ptr<ISceneNode>& node)
{	
	//if (const auto map = std::dynamic_pointer_cast<CMapTile>(node))
	//{
	//	if (false == map->GetMap().getTileIsCurrent(map->getIndexX(), map->getIndexZ()))
	//		return EVisitResult::AllowVisitChilds;
	//
	//	return __super::Visit(map);
	//}

	return EVisitResult::AllowVisitChilds;
}
