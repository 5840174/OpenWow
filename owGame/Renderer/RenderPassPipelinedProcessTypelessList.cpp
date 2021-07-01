#include "stdafx.h"

// General
#include "RenderPassPipelinedProcessTypelessList.h"

CRenderPassPipelinedProcessTypelessList::CRenderPassPipelinedProcessTypelessList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: RenderPassPipelined(RenderDevice)
	, m_CreateTypelessList(CreateTypelessList)
{
	m_PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(PerObject());

	for (size_t i = 0; i < (size_t)EShaderType::ComputeShader + 1ull; i++)
		m_PerObjectParameters[i] = nullptr;
}

CRenderPassPipelinedProcessTypelessList::~CRenderPassPipelinedProcessTypelessList()
{
}



//
// IRenderPassPipelined
//
void CRenderPassPipelinedProcessTypelessList::Render(RenderEventArgs & e)
{
	for (const auto& SceneNodeElement : GetSceneNodeListPass()->GetNodesList())
	{
		const_cast<ISceneNode*>(SceneNodeElement.SceneNode.get())->Accept(this);
	}
}



//
// IVisitor
//
EVisitResult CRenderPassPipelinedProcessTypelessList::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	if (false == SceneNode->IsEnabled())
		return EVisitResult::Block;

	BindPerObjectData(PerObject(SceneNode->GetWorldTransfom()));
	return EVisitResult::AllowVisitContent;
}

EVisitResult CRenderPassPipelinedProcessTypelessList::Visit(const std::shared_ptr<IUIControl>& UIControl)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassPipelinedProcessTypelessList::Visit(const std::shared_ptr<IModel>& Model)
{
	Model->Render(GetRenderEventArgs().PipelineState->GetShaders());
	return EVisitResult::AllowVisitChilds;
}

EVisitResult CRenderPassPipelinedProcessTypelessList::Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs)
{
	if (Material)
		Material->Bind(GetRenderEventArgs().PipelineState->GetPixelShaderPtr());

	Geometry->Render(GetRenderEventArgs().PipelineState->GetVertexShaderPtr(), GeometryDrawArgs);

	if (Material)
		Material->Unbind(GetRenderEventArgs().PipelineState->GetPixelShaderPtr());

	return EVisitResult::AllowVisitChilds;
}

EVisitResult CRenderPassPipelinedProcessTypelessList::Visit(const std::shared_ptr<ILight>& light)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassPipelinedProcessTypelessList::Visit(const std::shared_ptr<IParticleSystem>& ParticleSystem)
{
	return EVisitResult::Block;
}

EVisitResult CRenderPassPipelinedProcessTypelessList::Visit(const std::shared_ptr<ITerrain>& Terrain)
{
	return EVisitResult::Block;
}



//
// Protected
//
void CRenderPassPipelinedProcessTypelessList::BindPerObjectData(const PerObject& PerObject)
{
	m_PerObjectConstantBuffer->Set(PerObject);

	for (const auto& shadersIt : GetPipeline().GetShaders())
	{
		auto& perObjectParameter = m_PerObjectParameters[(size_t)shadersIt.first];
		if (perObjectParameter == nullptr)
			perObjectParameter = shadersIt.second->GetShaderParameterByName("PerObject");

		if (perObjectParameter && m_PerObjectConstantBuffer != nullptr)
		{
			perObjectParameter->SetConstantBuffer(m_PerObjectConstantBuffer);
			perObjectParameter->Bind();
		}		
	}
}

const std::shared_ptr<IRenderPassCreateTypelessList>& CRenderPassPipelinedProcessTypelessList::GetSceneNodeListPass() const
{
	return m_CreateTypelessList;
}

