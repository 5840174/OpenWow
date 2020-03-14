#include "stdafx.h"

// General
#include "RenderPass_M2.h"

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

CRenderPass_M2::CRenderPass_M2(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass, bool OpaqueDraw)
	: CBaseList3DPass(RenderDevice, SceneNodeListPass, cM2_NodeType)
	, m_CurrentM2Model(nullptr)
	, m_OpaqueDraw(OpaqueDraw)
{
	m_ADT_MDX_Distance = RenderDevice.GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetSettingT<float>("ADT_MDX_Distance");
}

CRenderPass_M2::~CRenderPass_M2()
{}



//
// CRenderPass_M2
//
void CRenderPass_M2::DoRenderM2Model(const CM2_Base_Instance* M2SceneNode, const CM2_Skin* M2Model, bool OpaqueDraw, UINT InstancesCnt)
{
	const ShaderMap& shaders = GetPipeline().GetShaders();
	const IShader* vertexShader = shaders.at(EShaderType::VertexShader).get();

	for (const auto& it : M2Model->GetTTT())
	{
		const auto& geom = M2Model->GetSections()[it.first];

		uint32 meshPartID = geom->getProto().meshPartID;
		if (!M2SceneNode->isMeshEnabled(meshPartID))
			continue;

		geom->UpdateGeometryProps(GetRenderEventArgs(), M2SceneNode);
		m_ShaderM2GeometryParameter->SetConstantBuffer(geom->GetGeometryPropsBuffer());
		m_ShaderM2GeometryParameter->Bind();

		m_ShaderM2GeometryBonesParameter->SetStructuredBuffer(geom->GetGeometryBonesBuffer());
		m_ShaderM2GeometryBonesParameter->Bind();
		{
			std::shared_ptr<IGeometryInternal> geomInternal = std::dynamic_pointer_cast<IGeometryInternal>(geom);
			geomInternal->Render_BindAllBuffers(GetRenderEventArgs(), vertexShader);


			for (const auto& it2 : it.second)
			{
				const auto& mat = it2;

				/*if (OpaqueDraw)
				{
					if (mat->GetM2Material()->getBlendMode() != 0 && mat->GetM2Material()->getBlendMode() != 1)
						continue;
				}
				else
				{
					if (mat->GetM2Material()->getBlendMode() == 0 || mat->GetM2Material()->getBlendMode() == 1)
						continue;
				}*/

				mat->GetM2Material()->GetBlendState()->Bind();
				mat->GetM2Material()->GetDepthStencilState()->Bind();

				mat->UpdateMaterialProps(GetRenderEventArgs(), M2SceneNode);
				mat->Bind(shaders);
				{
					SGeometryDrawArgs geometryDrawArgs = { 0 };
					geometryDrawArgs.IndexCnt = geom->getProto().indexCount;
					//geometryDrawArgs.VertexStartLocation = geom->getProto().vertexStart;
					geometryDrawArgs.VertexCnt = geom->getProto().vertexCount;
					geometryDrawArgs.InstanceCnt = InstancesCnt;
					geomInternal->Render_Draw(geometryDrawArgs);
				}
				mat->Unbind(shaders);
			}

			geomInternal->Render_UnbindAllBuffers(GetRenderEventArgs(), vertexShader);
		}
		m_ShaderM2GeometryBonesParameter->Unbind();

		m_ShaderM2GeometryParameter->Unbind();
	}
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_M2::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_D3D/M2.hlsl", "VS_main");
	//vertexShader->LoadInputLayoutFromReflector();
	std::vector<SCustomVertexElement> elements;
	elements.push_back({ 0, 0,  ECustomVertexElementType::FLOAT3, ECustomVertexElementUsage::POSITION, 0 });
	elements.push_back({ 0, 12,  ECustomVertexElementType::FLOAT4, ECustomVertexElementUsage::BLENDWEIGHT, 0 });
	elements.push_back({ 0, 28,  ECustomVertexElementType::FLOAT4, ECustomVertexElementUsage::BLENDINDICES, 0 });
	elements.push_back({ 0, 44, ECustomVertexElementType::FLOAT3, ECustomVertexElementUsage::NORMAL, 0 });
	elements.push_back({ 0, 56, ECustomVertexElementType::FLOAT2, ECustomVertexElementUsage::TEXCOORD, 0 });
	elements.push_back({ 0, 68, ECustomVertexElementType::FLOAT2, ECustomVertexElementUsage::TEXCOORD, 1 });
	vertexShader->LoadInputLayoutFromCustomElements(elements);

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "shaders_D3D/M2.hlsl", "PS_main");

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(alphaBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->SetShader(EShaderType::VertexShader, vertexShader);
	pipeline->SetShader(EShaderType::PixelShader, pixelShader);

	std::shared_ptr<ISamplerState> sampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	pipeline->SetSampler(0, sampler);
	pipeline->SetSampler(1, sampler);

	m_ShaderM2GeometryParameter = &vertexShader->GetShaderParameterByName("M2Geometry");
	_ASSERT(m_ShaderM2GeometryParameter->IsValid());

	m_ShaderM2GeometryBonesParameter = &vertexShader->GetShaderParameterByName("Bones");
	_ASSERT(m_ShaderM2GeometryBonesParameter->IsValid());

	return SetPipeline(pipeline);
}



//
// IVisitor
//
EVisitResult CRenderPass_M2::Visit(const ISceneNode3D* SceneNode3D)
{
	_ASSERT(SceneNode3D->Is(cM2_NodeType));

	if (const CM2_Base_Instance* m2Instance = static_cast<const CM2_Base_Instance*>(SceneNode3D))
	{
		m_CurrentM2Model = m2Instance;
		return CBaseList3DPass::Visit(m2Instance);
	}

	_ASSERT(false);
	return EVisitResult::Block;
}

EVisitResult CRenderPass_M2::Visit(const IModel* Model)
{
	if (const CM2_Skin* m2Skin = static_cast<const CM2_Skin*>(Model))
	{
		DoRenderM2Model(m_CurrentM2Model, m2Skin, m_OpaqueDraw);

		return EVisitResult::AllowAll;
	}

	return EVisitResult::Block;
}
