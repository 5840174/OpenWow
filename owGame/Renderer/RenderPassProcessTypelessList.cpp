#include "stdafx.h"

// General
#include "RenderPassProcessTypelessList.h"

CRenderPassProcessTypelessList::CRenderPassProcessTypelessList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: RenderPass(RenderDevice)
	, m_CreateTypelessList(CreateTypelessList)
{
}

CRenderPassProcessTypelessList::~CRenderPassProcessTypelessList()
{
}



//
// IRenderPassPipelined
//
void CRenderPassProcessTypelessList::Render(RenderEventArgs & e)
{
	for (const auto& SceneNodeElement : GetSceneNodeListPass()->GetNodesList())
	{
		const_cast<ISceneNode*>(SceneNodeElement.SceneNode.get())->Accept(this);
	}
}



//
// IVisitor
//
EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	if (false == SceneNode->IsEnabled())
		return EVisitResult::Block;

	return EVisitResult::AllowVisitContent;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<IUIControl>& UIControl)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<IModel>& Model)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<ILight>& light)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<IParticleSystem>& ParticleSystem)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<ITerrain>& Terrain)
{
	return EVisitResult::Block;
}



//
// Protected
//
const std::shared_ptr<IRenderPassCreateTypelessList>& CRenderPassProcessTypelessList::GetSceneNodeListPass() const
{
	return m_CreateTypelessList;
}

