#include "stdafx.h"

// General
#include "RenderPassProcessTypelessList.h"

CRenderPassProcessTypelessList::CRenderPassProcessTypelessList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: RenderPassPipelined(RenderDevice)
	, m_CreateTypelessList(CreateTypelessList)
	, m_PerObjectParameter(nullptr)
{
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject());
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

	BindPerObjectData(PerObject(SceneNode->GetWorldTransfom()));
	return EVisitResult::AllowVisitContent;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<IUIControl>& UIControl)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<IModel>& Model)
{
	Model->Render(GetRenderEventArgs().PipelineState->GetShaders());
	return EVisitResult::AllowVisitChilds;
}

EVisitResult CRenderPassProcessTypelessList::Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs)
{
	if (Material)
		Material->Bind(GetRenderEventArgs().PipelineState->GetPixelShaderPtr());

	Geometry->Render(GetRenderEventArgs().PipelineState->GetVertexShaderPtr(), GeometryDrawArgs);

	if (Material)
		Material->Unbind(GetRenderEventArgs().PipelineState->GetPixelShaderPtr());

	return EVisitResult::AllowVisitChilds;
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
void CRenderPassProcessTypelessList::BindPerObjectData(const PerObject& PerObject)
{
	m_PerObjectConstantBuffer->Set(PerObject);

	if (m_PerObjectParameter == nullptr)
		m_PerObjectParameter = (GetPipeline().GetVertexShaderPtr()->GetShaderParameterByName("PerObject"));

	if (m_PerObjectParameter && m_PerObjectConstantBuffer != nullptr)
	{
		m_PerObjectParameter->SetConstantBuffer(m_PerObjectConstantBuffer);
		m_PerObjectParameter->Bind();
	}
}

const std::shared_ptr<IRenderPassCreateTypelessList>& CRenderPassProcessTypelessList::GetSceneNodeListPass() const
{
	return m_CreateTypelessList;
}

