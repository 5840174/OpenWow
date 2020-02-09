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
	if (const CM2_Skin_Batch* m2Batch = dynamic_cast<const CM2_Skin_Batch*>(Model))
	{
		/*GetPipelineState()->GetBlendState().SetBlendMode(pMesh->GetM2Material()->GetBlendMode());
		GetPipelineState()->GetBlendState().Bind();

		GetPipelineState()->GetDepthStencilState().SetDepthMode(pMesh->GetM2Material()->GetDepthMode());
		GetPipelineState()->GetDepthStencilState().Bind();

		GetPipelineState()->GetRasterizerState().SetCullMode(pMesh->GetM2Material()->GetCullMode());
		GetPipelineState()->GetRasterizerState().Bind();

		GetPipelineState()->Bind();*/



		const SM2_SkinSection& skinProto = m2Batch->getSkin().getProto();

		uint32 meshPartID = skinProto.meshPartID;
		if (!m_CurrentM2Model->isMeshEnabled(meshPartID))
		{
			return false;
		}

		

		

		std::shared_ptr<CM2_Comp_Skeleton> skeleton = m_CurrentM2Model->getM2()->getSkeleton();

		

		const ShaderMap& shaders = GetPipeline().GetShaders();
		const IShader* vertexShader = shaders.at(EShaderType::VertexShader).get();

		for (const auto& connection : m2Batch->GetConnections())
		{
			M2_Material* m2Material = dynamic_cast<M2_Material*>(connection.Material.get());


			m2Material->SetBlendMode(static_cast<uint32>(m2Batch->m_M2ModelMaterial->getBlendMode()));

			bool isAnimated = skeleton->hasBones() && m_CurrentM2Model->getM2()->m_IsAnimated;
			m2Material->SetAnimated(isAnimated ? 1 : 0);
			if (isAnimated)
			{
				m2Material->SetMaxInfluences(skinProto.boneInfluences);

				//for (uint16 i = skinProto.bonesStartIndex; i < skinProto.bonesStartIndex + skinProto.boneCount; i++)
				//	skeleton->getBoneLookup(i)->SetNeedCalculate();

				//for (uint16 i = skinProto.bonesStartIndex; i < skinProto.bonesStartIndex + skinProto.boneCount; i++)
				//	skeleton->getBoneLookup(i)->calcMatrix(sceneNodeAsM2Instance->getAnimator()->getSequenceIndex(), sceneNodeAsM2Instance->getAnimator()->getCurrentTime(), static_cast<uint32>(renderEventArgs.TotalTime));

				//for (uint16 i = skinProto.bonesStartIndex; i < skinProto.bonesStartIndex + skinProto.boneCount; i++)
				//	skeleton->getBoneLookup(i)->calcBillboard(camera->GetViewMatrix(), sceneNodeAsM2Instance->GetWorldTransfom());

				std::vector<mat4> bones;
				for (uint16 i = skinProto.bonesStartIndex; i < skinProto.bonesStartIndex + skinProto.boneCount; i++)
				{
					_ASSERT(skeleton->isLookupBoneCorrect(i));
					bones.push_back(skeleton->getBoneLookup(i)->getTransformMatrix());
				}

				m2Material->SetBones(bones);
			}

			// Shader
			m2Material->SetNewShader(/*newShader*/0);

			// Model color
			bool isColorEnable = (m2Batch->m_Color != nullptr);
			m2Material->SetColorEnable(isColorEnable);
			if (isColorEnable)
			{
				m2Material->SetColor(m2Batch->m_Color->getValue());
			}

			// Textures
			for (uint32 i = 0; i < m2Batch->m_Textures.size(); i++)
			{
				std::shared_ptr<const CM2_Part_Texture> m2Texture = m2Batch->m_Textures[i].lock();

				m2Material->SetTexture(i, m2Texture->GetResultTexture(m_CurrentM2Model));
				//m2Material->GetSampler(i)->SetWrapMode(m2Texture->GetTextureWrapX(), m2Texture->GetTextureWrapY());
			}

			// Texture alpha
			bool isTextureWeightEnable = (m2Batch->m_TextureWeight != nullptr);
			m2Material->SetTextureWeightEnable(isTextureWeightEnable);
			if (isTextureWeightEnable)
			{
				m2Material->SetTextureWeight(m2Batch->m_TextureWeight->getValue());
			}

			// Texture transform
			bool isTextureTransformEnable = (m2Batch->m_TextureTransform != nullptr);
			m2Material->SetTextureAnimEnable(isTextureTransformEnable);
			if (isTextureTransformEnable)
			{
				m2Material->SetTextureAnimMatrix(m2Batch->m_TextureTransform->getValue());
			}

			SGeometryDrawArgs geometryDrawArgs = connection.GeometryDrawArgs;
			geometryDrawArgs.IndexCnt = skinProto.indexCount;
			geometryDrawArgs.VertexCnt = skinProto.vertexCount;

			m2Material->Bind(shaders);
			connection.Geometry->Render(GetRenderEventArgs(), vertexShader, connection.GeometryDrawArgs);
			m2Material->Unbind(shaders);
		}

		return Base3DPass::Visit(m2Batch);
	}

    return false;
}
