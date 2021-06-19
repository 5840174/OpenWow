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
	//m_ModelsList.clear();
	//m_GeometryList.clear();
	//m_LightList.clear();
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
	//_ASSERT(! m_NodesList.empty());
	//const auto& lastNodeElement = m_NodesList.back();
	//lastNodeElement.ModelsElements.push_back(SModelElement(lastNodeElement.SceneNode, Model));
	//m_ModelsList.push_back(SModelElement(lastNodeElement.SceneNode, Model));
	//return EVisitResult::AllowAll;
	return EVisitResult::Block;
}

EVisitResult CRenderPassCreateTypelessList::Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs)
{
	//_ASSERT(! m_ModelsList.empty());
	//const auto& lastModelElement = m_ModelsList.back();
	//lastModelElement.GeometriesElements.push_back(SGeometryElement(lastModelElement.SceneNode, lastModelElement.Model, Geometry, Material, GeometryDrawArgs));
	//m_GeometryList.push_back(SGeometryElement(lastModelElement.SceneNode, lastModelElement.Model, Geometry, Material, GeometryDrawArgs));
	//return EVisitResult::AllowAll;
	return EVisitResult::Block;
}

EVisitResult CRenderPassCreateTypelessList::Visit(const std::shared_ptr<ILight>& light)
{
	//_ASSERT(! m_NodesList.empty());
	//const auto& lastNodeElement = m_NodesList.back();
	//lastNodeElement.LightsElement.push_back(SLightElement(lastNodeElement.SceneNode, light));
	//m_LightList.push_back(SLightElement(lastNodeElement.SceneNode, light));
	//return EVisitResult::AllowAll;
	return EVisitResult::Block;
}
