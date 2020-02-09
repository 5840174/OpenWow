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

CRenderPass_M2::CRenderPass_M2(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
	, m_CurrentM2Model(nullptr)
{}

CRenderPass_M2::~CRenderPass_M2()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_M2::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_D3D/M2.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "shaders_D3D/M2.hlsl", "PS_main");

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(alphaBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->GetRasterizerState()->SetViewport(Viewport);
	pipeline->SetShader(EShaderType::VertexShader, vertexShader);
	pipeline->SetShader(EShaderType::PixelShader, pixelShader);

	std::shared_ptr<ISamplerState> sampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	pipeline->SetSampler(0, sampler);
	pipeline->SetSampler(1, sampler);

	m_ShaderM2GeometryParameter = &vertexShader->GetShaderParameterByName("M2Geometry");
	_ASSERT(m_ShaderM2GeometryParameter->IsValid());

	return SetPipeline(pipeline);
}



//
// IVisitor
//
bool CRenderPass_M2::Visit(const ISceneNode3D* node)
{
    if (const CM2_Base_Instance* m2Instance = dynamic_cast<const CM2_Base_Instance*>(node))
    {
		m_CurrentM2Model = m2Instance;
        return Base3DPass::Visit(m2Instance);
    }

    return false;
}

bool CRenderPass_M2::Visit(const IModel* Model)
{
	if (const CM2_Skin* m2Skin = dynamic_cast<const CM2_Skin*>(Model))
	{
		/*GetPipelineState()->GetBlendState().SetBlendMode(pMesh->GetM2Material()->GetBlendMode());
		GetPipelineState()->GetBlendState().Bind();

		GetPipelineState()->GetDepthStencilState().SetDepthMode(pMesh->GetM2Material()->GetDepthMode());
		GetPipelineState()->GetDepthStencilState().Bind();

		GetPipelineState()->GetRasterizerState().SetCullMode(pMesh->GetM2Material()->GetCullMode());
		GetPipelineState()->GetRasterizerState().Bind();

		GetPipelineState()->Bind();*/

		const ShaderMap& shaders = GetPipeline().GetShaders();
		const IShader* vertexShader = shaders.at(EShaderType::VertexShader).get();

		for (const auto& it : m2Skin->GetTTT())
		{
			const auto& geom = it.first;
			std::shared_ptr<IGeometryInternal> geomInternal = std::dynamic_pointer_cast<IGeometryInternal>(geom);

			uint32 meshPartID = geom->getProto().meshPartID;
			if (!m_CurrentM2Model->isMeshEnabled(meshPartID))
				continue;

			geom->UpdateGeometryProps(m_CurrentM2Model);
			m_ShaderM2GeometryParameter->SetConstantBuffer(geom->GetGeometryPropsBuffer());
			m_ShaderM2GeometryParameter->Bind();
			{
				
				geomInternal->Render_BindAllBuffers(GetRenderEventArgs(), vertexShader);


				for (const auto& it2 : it.second)
				{
					const auto& mat = it2;

					mat->UpdateMaterialProps(m_CurrentM2Model);
					mat->Bind(shaders);
					{
						SGeometryDrawArgs geometryDrawArgs = { 0 };
						geometryDrawArgs.IndexCnt = geom->getProto().indexCount;
						geometryDrawArgs.VertexCnt = geom->getProto().vertexCount;
						geomInternal->Render_Draw(geometryDrawArgs);
					}
					mat->Unbind(shaders);
				}

				geomInternal->Render_UnbindAllBuffers(GetRenderEventArgs(), vertexShader);
			}
			m_ShaderM2GeometryParameter->Unbind();
		}

		return true;
	}

    return false;
}
