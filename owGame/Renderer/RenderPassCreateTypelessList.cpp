#include "stdafx.h"

// General
#include "RenderPassCreateTypelessList.h"


CRenderPassCreateTypelessList::CRenderPassCreateTypelessList(IRenderDevice& RenderDevice, IScene& Scene)
	: ScenePass(Scene)
{
	SetPassName("CreateTypelessList");
}

CRenderPassCreateTypelessList::~CRenderPassCreateTypelessList()
{}



//
// CRenderPassCreateTypelessList
//
const std::vector<IRenderPassCreateTypelessList::SNodeElement>& CRenderPassCreateTypelessList::GetNodesList() const
{
	return m_NodesList;
}

const std::vector<IRenderPassCreateTypelessList::SModelElement>& CRenderPassCreateTypelessList::GetModelsList() const
{
	throw CException("Not implemented.");
}

const std::vector<IRenderPassCreateTypelessList::SGeometryElement>& CRenderPassCreateTypelessList::GetGeometryList() const
{
	throw CException("Not implemented.");
}

const std::vector<IRenderPassCreateTypelessList::SLightElement>& CRenderPassCreateTypelessList::GetLightList() const
{
	throw CException("Not implemented.");
}

//
// IRenderPass
//
void CRenderPassCreateTypelessList::PreRender(RenderEventArgs & e)
{
	__super::PreRender(e);

	m_NodesList.clear();
}

void CRenderPassCreateTypelessList::Render(RenderEventArgs & e)
{
	__super::Render(e);
}



//
// IVisitor
//
EVisitResult CRenderPassCreateTypelessList::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	if (auto colliderComponent = SceneNode->GetComponentT<IColliderComponent>())
		if (colliderComponent->IsCulled(GetRenderEventArgs().CameraForCulling))
			return EVisitResult::Block;

	m_NodesList.push_back(SNodeElement(SceneNode));
	return EVisitResult::AllowVisitChilds;
}

EVisitResult CRenderPassCreateTypelessList::Visit(const std::shared_ptr<IModel>& Model)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassCreateTypelessList::Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassCreateTypelessList::Visit(const std::shared_ptr<ILight>& light)
{
	return EVisitResult::Block;
}
