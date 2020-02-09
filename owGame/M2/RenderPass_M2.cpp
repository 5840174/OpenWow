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

	m_ShaderM2GeometryBonesParameter = &vertexShader->GetShaderParameterByName("Bones");
	_ASSERT(m_ShaderM2GeometryBonesParameter->IsValid());

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
		const ShaderMap& shaders = GetPipeline().GetShaders();
		const IShader* vertexShader = shaders.at(EShaderType::VertexShader).get();

		for (const auto& it : m2Skin->GetTTT())
		{
			const auto& geom = it.first;
			std::shared_ptr<IGeometryInternal> geomInternal = std::dynamic_pointer_cast<IGeometryInternal>(geom);

			uint32 meshPartID = geom->getProto().meshPartID;
			if (!m_CurrentM2Model->isMeshEnabled(meshPartID))
				continue;

			geom->UpdateGeometryProps(GetRenderEventArgs(), m_CurrentM2Model);
			m_ShaderM2GeometryParameter->SetConstantBuffer(geom->GetGeometryPropsBuffer());
			m_ShaderM2GeometryParameter->Bind();

			m_ShaderM2GeometryBonesParameter->SetStructuredBuffer(geom->GetGeometryBonesBuffer());
			m_ShaderM2GeometryBonesParameter->Bind();

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
			m_ShaderM2GeometryBonesParameter->Unbind();

			m_ShaderM2GeometryParameter->Unbind();
		}

		return true;
	}

    return false;
}


bool CRenderPass_M2::VisitInstanced(const IModel* Model, size_t cnt)
{
	if (const CM2_Skin* m2Skin = dynamic_cast<const CM2_Skin*>(Model))
	{
		const ShaderMap& shaders = GetPipeline().GetShaders();
		const IShader* vertexShader = shaders.at(EShaderType::VertexShader).get();

		for (const auto& it : m2Skin->GetTTT())
		{
			const auto& geom = it.first;
			std::shared_ptr<IGeometryInternal> geomInternal = std::dynamic_pointer_cast<IGeometryInternal>(geom);

			uint32 meshPartID = geom->getProto().meshPartID;
			if (!m_CurrentM2Model->isMeshEnabled(meshPartID))
				continue;

			geom->UpdateGeometryProps(GetRenderEventArgs(), m_CurrentM2Model);
			m_ShaderM2GeometryParameter->SetConstantBuffer(geom->GetGeometryPropsBuffer());
			m_ShaderM2GeometryParameter->Bind();

			m_ShaderM2GeometryBonesParameter->SetStructuredBuffer(geom->GetGeometryBonesBuffer());
			m_ShaderM2GeometryBonesParameter->Bind();

			{

				geomInternal->Render_BindAllBuffers(GetRenderEventArgs(), vertexShader);


				for (const auto& it2 : it.second)
				{
					const auto& mat = it2;

					mat->UpdateMaterialProps(m_CurrentM2Model);
					mat->Bind(shaders);
					{
						SGeometryDrawInstancedArgs GeometryDrawInstancedArgs;
						GeometryDrawInstancedArgs.IndexCnt = geom->getProto().indexCount;
						GeometryDrawInstancedArgs.VertexCnt = geom->getProto().vertexCount;
						GeometryDrawInstancedArgs.InstanceCnt = cnt;
						geomInternal->Render_DrawInstanced(GeometryDrawInstancedArgs);
					}
					mat->Unbind(shaders);
				}

				geomInternal->Render_UnbindAllBuffers(GetRenderEventArgs(), vertexShader);
			}
			m_ShaderM2GeometryBonesParameter->Unbind();

			m_ShaderM2GeometryParameter->Unbind();
		}

		return true;
	}

	return false;
}



//----------------------------------------------------------------------



CRenderPass_M2_Instanced::CRenderPass_M2_Instanced(IRenderDevice & RenderDevice, const std::shared_ptr<BuildRenderListPassTemplated<CM2_Base_Instance>>& List, std::shared_ptr<IScene> scene)
	: CRenderPass_M2(RenderDevice, scene)
	, m_RenderListPass(List)
{
}

CRenderPass_M2_Instanced::~CRenderPass_M2_Instanced()
{
}

void CRenderPass_M2_Instanced::Render(RenderEventArgs & e)
{
	std::unordered_map<const IModel*, std::vector<const ISceneNode3D*>> modelPriorMap;

	for (const auto& it : m_RenderListPass->GetModelsList())
	{
		auto& resultIter = modelPriorMap.find(it.Model);
		if (resultIter == modelPriorMap.end())
		{
			modelPriorMap.insert(std::make_pair(it.Model, std::vector<const ISceneNode3D*>({it.Node})));
		}
		else
		{
			resultIter->second.push_back(it.Node);
		}
	}


	for (const auto& it : modelPriorMap)
	{
		std::vector<glm::mat4> instances;
		std::for_each(it.second.begin(), it.second.end(), [&instances](const ISceneNode3D* sceneNode) {instances.push_back(sceneNode->GetWorldTransfom()); });

		std::shared_ptr<IStructuredBuffer> sbBuffer = GetRenderDevice().GetObjectsFactory().CreateStructuredBuffer(instances);
		m_ShaderInstancesBufferParameter->SetStructuredBuffer(sbBuffer);
		m_ShaderInstancesBufferParameter->Bind();

		m_CurrentM2Model = dynamic_cast<const CM2_Base_Instance*>(*it.second.begin());
		CRenderPass_M2::VisitInstanced(it.first, it.second.size());

		m_ShaderInstancesBufferParameter->Unbind();
	}


	Log::Info("Test = '%d'", modelPriorMap.size());

}

std::shared_ptr<IRenderPassPipelined> CRenderPass_M2_Instanced::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_D3D/M2.hlsl", "VS_main_Inst");
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

	m_ShaderInstancesBufferParameter = &vertexShader->GetShaderParameterByName("Instances");
	_ASSERT(m_ShaderInstancesBufferParameter->IsValid());

	m_ShaderM2GeometryParameter = &vertexShader->GetShaderParameterByName("M2Geometry");
	_ASSERT(m_ShaderM2GeometryParameter->IsValid());

	m_ShaderM2GeometryBonesParameter = &vertexShader->GetShaderParameterByName("Bones");
	_ASSERT(m_ShaderM2GeometryBonesParameter->IsValid());

	return SetPipeline(pipeline);
}

bool CRenderPass_M2_Instanced::Visit(const ISceneNode3D * node)
{
	_ASSERT(false);
	return false;
}

bool CRenderPass_M2_Instanced::Visit(const IModel * Model)
{
	_ASSERT(false);
	return false;
}
