#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "RenderPass_M2Instanced.h"

// Additional
#include "M2_Part_Color.h"
#include "M2_Part_Material.h"
#include "M2_Part_Texture.h"
#include "M2_Part_TextureTransform.h"
#include "M2_Part_TextureWeight.h"

// Additional (SceneNodes)
#include "M2_Base_Instance.h"

// Additional (meshes)
#include "M2_Skin_Batch.h"


CRenderPass_M2_Instanced::CRenderPass_M2_Instanced(IRenderDevice & RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneCreateTypedListsPass, bool OpaqueDraw)
	: CRenderPass_M2(RenderDevice, SceneCreateTypedListsPass, OpaqueDraw)
{
	m_InstancesBuffer = GetRenderDevice().GetObjectsFactory().CreateStructuredBuffer(nullptr, 1000, sizeof(M2PerObject), CPUAccess::Write);
}

CRenderPass_M2_Instanced::~CRenderPass_M2_Instanced()
{
}

void CRenderPass_M2_Instanced::Render(RenderEventArgs & e)
{
	std::unordered_map<const CM2_Skin*, std::vector<const CM2_Base_Instance*>> modelPriorMap;

	for (const auto& acceptableNodeType : GetAcceptableNodeTypes())
	{
		if (GetSceneNodeListPass()->HasModelsList(acceptableNodeType))
		{
			for (const auto& it : GetSceneNodeListPass()->GetModelsList(acceptableNodeType))
			{
				modelPriorMap[static_cast<const CM2_Skin*>(it.Model)].push_back(static_cast<const CM2_Base_Instance*>(it.SceneNode));
			}
		}
	}

	for (const auto& it : modelPriorMap)
	{
		std::vector<M2PerObject> instances;
		instances.reserve(it.second.size());
		std::for_each(it.second.begin(), it.second.end(), [&instances](const CM2_Base_Instance* sceneNode) {
			instances.push_back(M2PerObject(sceneNode->GetWorldTransfom(), sceneNode->getColor())); 
		});

		if (instances.size() > m_InstancesBuffer->GetElementCount())
			m_InstancesBuffer = GetRenderDevice().GetObjectsFactory().CreateStructuredBuffer(instances, CPUAccess::Write);
		else
			m_InstancesBuffer->Set(instances);

		m_ShaderInstancesBufferParameter->SetStructuredBuffer(m_InstancesBuffer);
		m_ShaderInstancesBufferParameter->Bind();
		{
			DoRenderM2Model(*it.second.begin(), it.first, m_OpaqueDraw, instances.size());
		}
		m_ShaderInstancesBufferParameter->Unbind();
	}


	//Log::Info("Test = '%d'", modelPriorMap.size());

}

std::shared_ptr<IRenderPassPipelined> CRenderPass_M2_Instanced::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/M2.hlsl", "VS_main_Inst");
	//vertexShader->LoadInputLayoutFromReflector();
	std::vector<SCustomVertexElement> elements;
	elements.push_back({ 0, 0,  ECustomVertexElementType::FLOAT3, ECustomVertexElementUsage::POSITION, 0 });
	elements.push_back({ 0, 12,  ECustomVertexElementType::FLOAT4, ECustomVertexElementUsage::BLENDWEIGHT, 0 });
	elements.push_back({ 0, 28,  ECustomVertexElementType::FLOAT4, ECustomVertexElementUsage::BLENDINDICES, 0 });
	elements.push_back({ 0, 44, ECustomVertexElementType::FLOAT3, ECustomVertexElementUsage::NORMAL, 0 });
	elements.push_back({ 0, 56, ECustomVertexElementType::FLOAT2, ECustomVertexElementUsage::TEXCOORD, 0 });
	elements.push_back({ 0, 68, ECustomVertexElementType::FLOAT2, ECustomVertexElementUsage::TEXCOORD, 1 });
	vertexShader->LoadInputLayoutFromCustomElements(elements);

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/M2.hlsl", "PS_main");

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(alphaBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->SetShader(EShaderType::VertexShader, vertexShader);
	pipeline->SetShader(EShaderType::PixelShader, pixelShader);

	std::shared_ptr<ISamplerState> sampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	pipeline->SetSampler(0, sampler);
	pipeline->SetSampler(1, sampler);

	m_ShaderInstancesBufferParameter = &vertexShader->GetShaderParameterByName("Instances");
	_ASSERT(m_ShaderInstancesBufferParameter->IsValid());

	m_ShaderM2GeometryParameter = &vertexShader->GetShaderParameterByName("M2Geometry");
	_ASSERT(m_ShaderM2GeometryParameter->IsValid());

	m_ShaderM2GeometryBonesParameter = &vertexShader->GetShaderParameterByName("Bones");
	_ASSERT(m_ShaderM2GeometryBonesParameter->IsValid());

	return SetPipeline(pipeline);
}

EVisitResult CRenderPass_M2_Instanced::Visit(const ISceneNode * node)
{
	_ASSERT(false);
	return EVisitResult::Block;
}

EVisitResult CRenderPass_M2_Instanced::Visit(const IModel * Model)
{
	_ASSERT(false);
	return EVisitResult::Block;
}

#endif
