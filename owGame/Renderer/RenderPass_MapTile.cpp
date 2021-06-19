#include "stdafx.h"

// General
#include "RenderPass_MapTile.h"

// Additional
#include "Map/Map.h"
#include "Map/MapTile.h"

CRenderPass_MapTile::CRenderPass_MapTile(IScene& Scene, const std::shared_ptr<IRenderPassCreateTypelessList>& SceneListTypeless)
	: Base3DPass(Scene)
	, m_SceneListTypeless(SceneListTypeless)
{
	SetPassName("MapTile");

	m_WoWSettings = GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_MapTile::~CRenderPass_MapTile()
{}



//
// IVisitor
//
EVisitResult CRenderPass_MapTile::Visit(const std::shared_ptr<ISceneNode>& node)
{	
	if (const auto map = std::dynamic_pointer_cast<CMapTile>(node))
	{
		if (false == map->GetMap().getTileIsCurrent(map->getIndexX(), map->getIndexZ()))
			return EVisitResult::Block;

		if (auto colliderComponent = node->GetComponentT<IColliderComponent>())
			if (false == colliderComponent->IsCulled(GetRenderEventArgs().Camera))
				return EVisitResult::Block;
	
		return __super::Visit(map);
	}

	return EVisitResult::AllowVisitChilds;
}
