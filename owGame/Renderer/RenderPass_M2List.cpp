#include "stdafx.h"

// Include
#include "RendererWoW.h"

// General
#include "RenderPass_M2List.h"

// Additional
#include "M2/M2_Part_Color.h"
#include "M2/M2_Part_Material.h"
#include "M2/M2_Part_Texture.h"
#include "M2/M2_Part_TextureTransform.h"
#include "M2/M2_Part_TextureWeight.h"

// Additional (meshes)
#include "M2/M2_Skin_Batch.h"


CRenderPass_M2List::CRenderPass_M2List(CRendererWoW& RendererWoW, IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList, ERenderPassM2DrawMode DrawMode)
	: CRenderPassPipelinedProcessTypelessList(RenderDevice, CreateTypelessList)
	, m_RendererWoW(RendererWoW)
	, m_DrawMode(DrawMode)
	, m_CurrentM2Instance(nullptr)
{
	SetPassName("M2List " + std::string((DrawMode == ERenderPassM2DrawMode::Opaque) ? "Opaque" : (DrawMode == ERenderPassM2DrawMode::Transperent) ? "Transperent" : "All"));

	m_M2PerObjectConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(nullptr, sizeof(M2PerObject));
}

CRenderPass_M2List::~CRenderPass_M2List()
{}



//
// CRenderPass_M2List
//
void CRenderPass_M2List::DoRenderM2Model(const CM2_Base_Instance* M2SceneNode, const CM2_Skin* M2Model, UINT InstancesCnt)
{
	for (const auto& geometryMaterialsIt : M2Model->GetGeometryMaterials())
	{
		const auto& geometry = M2Model->GetGeometries()[geometryMaterialsIt.first];
		const auto& materials = geometryMaterialsIt.second;

		uint32 meshPartID = geometry->getProto().meshPartID;
		if (false == M2SceneNode->IsMeshEnabled(meshPartID))
			continue;

		geometry->UpdateGeometryProps(GetRenderEventArgs(), M2SceneNode);

		m_ShaderM2GeometryParameter->SetConstantBuffer(geometry->GetGeometryPropsBuffer());
		m_ShaderM2GeometryParameter->Bind();
		{
			m_ShaderM2GeometryBonesParameter->SetStructuredBuffer(geometry->GetGeometryBonesBuffer());
			m_ShaderM2GeometryBonesParameter->Bind();
			{
				auto geomInternal = std::dynamic_pointer_cast<IGeometryInternal>(geometry);
				geomInternal->Render_BindAllBuffers(GetPipeline().GetVertexShaderPtr());

				for (const auto& materialsIt : materials)
				{
					const auto& material = materialsIt;

					bool isOpaqueGeom = (material->GetM2Material()->getBlendMode() == 0) || (material->GetM2Material()->getBlendMode() == 1);
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

					auto blendMode = m_RendererWoW.GetEGxBlend(material->GetM2Material()->getBlendMode());

					blendMode->Bind();
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

					material->GetM2Material()->GetRasterizerState()->Unbind();
					material->GetM2Material()->GetDepthStencilState()->Unbind();
					blendMode->Unbind();
				}

				geomInternal->Render_UnbindAllBuffers(GetPipeline().GetVertexShaderPtr());
			}
			m_ShaderM2GeometryBonesParameter->Unbind();
		}
		m_ShaderM2GeometryParameter->Unbind();
	}
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_M2List::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
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

	GetPipeline().SetRenderTarget(RenderTarget);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

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
EVisitResult CRenderPass_M2List::Visit(const std::shared_ptr<ISceneNode>& SceneNode3D)
{
	m_CurrentM2Instance = nullptr;

	if (auto m2Instance = std::dynamic_pointer_cast<CM2_Base_Instance>(SceneNode3D))
	{
		if (false == m2Instance->IsInstansingEnabled())
		{
			m_CurrentM2Instance = m2Instance.get();

			M2PerObject perObject(m2Instance->GetWorldTransfom(), m2Instance->GetColor());
			m_M2PerObjectConstantBuffer->Set(perObject);

			if (m_ShaderM2PerObjectParameter && m_M2PerObjectConstantBuffer != nullptr)
			{
				m_ShaderM2PerObjectParameter->SetConstantBuffer(m_M2PerObjectConstantBuffer);
				m_ShaderM2PerObjectParameter->Bind();
			}

			return EVisitResult::AllowVisitContent;
		}
	}

	return EVisitResult::Block;
}

EVisitResult CRenderPass_M2List::Visit(const std::shared_ptr<IModel>& Model)
{
	if (auto m2Skin = std::dynamic_pointer_cast<CM2_Skin>(Model))
	{
		DoRenderM2Model(m_CurrentM2Instance, m2Skin.get());
		return EVisitResult::AllowAll;
	}

	_ASSERT(false);
	return EVisitResult::Block;
}
