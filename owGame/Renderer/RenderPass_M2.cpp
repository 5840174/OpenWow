#include "stdafx.h"

// General
#include "RenderPass_M2.h"

// Additional
#include "M2/M2_Part_Color.h"
#include "M2/M2_Part_Material.h"
#include "M2/M2_Part_Texture.h"
#include "M2/M2_Part_TextureTransform.h"
#include "M2/M2_Part_TextureWeight.h"

// Additional (SceneNodes)
#include "M2/M2_Base_Instance.h"

// Additional (meshes)
#include "M2/M2_Skin_Batch.h"

CRenderPass_M2::CRenderPass_M2(IScene& Scene, ERenderPassM2DrawMode DrawMode)
	: Base3DPass(Scene)
	, m_CurrentM2Model(nullptr)
	, m_DrawMode(DrawMode)
{
	SetPassName("M2");

	m_ADT_MDX_Distance = Scene.GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("ADT_MDX_Distance");

	m_M2PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(nullptr, sizeof(M2PerObject));
}

CRenderPass_M2::~CRenderPass_M2()
{}



//
// CRenderPass_M2
//
void CRenderPass_M2::DoRenderM2Model(const CM2_Base_Instance* M2SceneNode, const CM2_Skin* M2Model, UINT InstancesCnt)
{
	const ShaderMap& shaders = GetPipeline().GetShaders();
	const IShader* vertexShader = shaders.at(EShaderType::VertexShader).get();

	for (const auto& geometryMaterialsIt : M2Model->GetGeometryMaterials())
	{
		const auto& geometry = M2Model->GetGeometries()[geometryMaterialsIt.first];
		const auto& materials = geometryMaterialsIt.second;

		uint32 meshPartID = geometry->getProto().meshPartID;
		if (false == M2SceneNode->isMeshEnabled(meshPartID))
			continue;

		geometry->UpdateGeometryProps(GetRenderEventArgs(), M2SceneNode);

		m_ShaderM2GeometryParameter->SetConstantBuffer(geometry->GetGeometryPropsBuffer());
		m_ShaderM2GeometryParameter->Bind();
		
		m_ShaderM2GeometryBonesParameter->SetStructuredBuffer(geometry->GetGeometryBonesBuffer());
		m_ShaderM2GeometryBonesParameter->Bind();
		{
			auto geomInternal = std::dynamic_pointer_cast<IGeometryInternal>(geometry);
			geomInternal->Render_BindAllBuffers(vertexShader);

			for (const auto& materialsIt : materials)
			{
				const auto& material = materialsIt;

				bool isOpaqueGeom = material->GetM2Material()->getBlendMode() == 0 || material->GetM2Material()->getBlendMode() == 1;
				switch (m_DrawMode)
				{
					case ERenderPassM2DrawMode::Opaque:
					{
						if (false == isOpaqueGeom)
							continue;
					}
					break;
					case ERenderPassM2DrawMode::Transperent:
					{
						if (isOpaqueGeom)
							continue;
					}
					break;
				}

				material->GetM2Material()->GetBlendState()->Bind();
				material->GetM2Material()->GetDepthStencilState()->Bind();
				material->GetM2Material()->GetRasterizerState()->Bind();

				material->UpdateMaterialProps(GetRenderEventArgs(), M2SceneNode);
				material->Bind(GetPipeline().GetPixelShaderPtr());
				{
					SGeometryDrawArgs geometryDrawArgs = { 0 };
					//geometryDrawArgs.IndexStartLocation = geom->getProto().indexStart;
					geometryDrawArgs.IndexCnt = geometry->getProto().indexCount;
					//geometryDrawArgs.VertexStartLocation = geom->getProto().vertexStart;
					geometryDrawArgs.VertexCnt = geometry->getProto().vertexCount;
					geometryDrawArgs.InstanceCnt = InstancesCnt;
					geomInternal->Render_Draw(geometryDrawArgs);
				}
				material->Unbind(GetPipeline().GetPixelShaderPtr());
			}

			geomInternal->Render_UnbindAllBuffers(vertexShader);
		}
		m_ShaderM2GeometryBonesParameter->Unbind();

		m_ShaderM2GeometryParameter->Unbind();
	}
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_M2::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/M2.hlsl", "VS_main");
	//vertexShader->LoadInputLayoutFromReflector();
	std::vector<SCustomInputElement> elements;
	elements.push_back({ 0, 0,  ECustomVertexElementType::FLOAT3, ECustomVertexElementUsage::POSITION, 0 });
	elements.push_back({ 0, 12,  ECustomVertexElementType::FLOAT4, ECustomVertexElementUsage::BLENDWEIGHT, 0 });
	elements.push_back({ 0, 28,  ECustomVertexElementType::FLOAT4, ECustomVertexElementUsage::BLENDINDICES, 0 });
	elements.push_back({ 0, 44, ECustomVertexElementType::FLOAT3, ECustomVertexElementUsage::NORMAL, 0 });
	elements.push_back({ 0, 56, ECustomVertexElementType::FLOAT2, ECustomVertexElementUsage::TEXCOORD, 0 });
	elements.push_back({ 0, 68, ECustomVertexElementType::FLOAT2, ECustomVertexElementUsage::TEXCOORD, 1 });
	vertexShader->LoadInputLayoutFromCustomElements(elements);

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/M2.hlsl", "PS_main");

	// PIPELINES
	GetPipeline().SetRenderTarget(RenderTarget);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

	//std::shared_ptr<ISamplerState> sampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	//sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	//pipeline->SetSampler(0, sampler);
	//pipeline->SetSampler(1, sampler);

	m_ShaderM2PerObjectParameter = vertexShader->GetShaderParameterByName("M2PerObject");
	_ASSERT(m_ShaderM2PerObjectParameter != nullptr);

	m_ShaderM2GeometryParameter = vertexShader->GetShaderParameterByName("M2Geometry");
	_ASSERT(m_ShaderM2GeometryParameter != nullptr);

	m_ShaderM2GeometryBonesParameter = vertexShader->GetShaderParameterByName("Bones");
	_ASSERT(m_ShaderM2GeometryBonesParameter != nullptr);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_M2::Visit(const std::shared_ptr<ISceneNode>& SceneNode3D)
{
	if (auto m2Instance = std::dynamic_pointer_cast<CM2_Base_Instance>(SceneNode3D))
	{
		m_CurrentM2Model = m2Instance.get();

		M2PerObject perObject(m2Instance->GetWorldTransfom(), m2Instance->getColor());
		m_M2PerObjectConstantBuffer->Set(perObject);

		if (m_ShaderM2PerObjectParameter && m_M2PerObjectConstantBuffer != nullptr)
		{
			m_ShaderM2PerObjectParameter->SetConstantBuffer(m_M2PerObjectConstantBuffer);
			m_ShaderM2PerObjectParameter->Bind();
		}

		return EVisitResult::AllowAll;
	}

	return EVisitResult::AllowVisitChilds;
}

EVisitResult CRenderPass_M2::Visit(const std::shared_ptr<IModel>& Model)
{
	if (auto m2Skin = std::dynamic_pointer_cast<CM2_Skin>(Model))
	{
		DoRenderM2Model(m_CurrentM2Model, m2Skin.get());
		return EVisitResult::AllowAll;
	}

	_ASSERT(false);
	return EVisitResult::Block;
}
