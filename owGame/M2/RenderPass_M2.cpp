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

CRenderPass_M2::CRenderPass_M2(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene, bool OpaqueDraw)
	: Base3DPass(RenderDevice, scene)
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
		const auto& geom = it.first;

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

				if (OpaqueDraw)
				{
					if (mat->GetM2Material()->getBlendMode() != 0 && mat->GetM2Material()->getBlendMode() != 1)
						continue;
				}
				else
				{
					if (mat->GetM2Material()->getBlendMode() == 0 || mat->GetM2Material()->getBlendMode() == 1)
						continue;
				}

				mat->GetM2Material()->GetBlendState()->Bind();
				//mat->GetM2Material()->GetDepthStencilState()->Bind();

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
bool CRenderPass_M2::Visit(const ISceneNode3D* SceneNode3D)
{
	if (const CM2_Base_Instance* m2Instance = dynamic_cast<const CM2_Base_Instance*>(SceneNode3D))
	{
		const auto& collider = SceneNode3D->GetComponent<IColliderComponent3D>();

		const ICameraComponent3D* camera = GetRenderEventArgs().CameraForCulling;
		_ASSERT(camera != nullptr);

		if (collider->IsCulledByDistance2D(camera, m_ADT_MDX_Distance->Get()))
			return false;

		if (collider->IsCulledByFrustum(camera))
			return false;

		m_CurrentM2Model = m2Instance;
		return Base3DPass::Visit(m2Instance);
	}

	return false;
}

bool CRenderPass_M2::Visit(const IModel* Model)
{
	if (const CM2_Skin* m2Skin = dynamic_cast<const CM2_Skin*>(Model))
	{
		DoRenderM2Model(m_CurrentM2Model, m2Skin, m_OpaqueDraw);

		return true;
	}

	return false;
}


//----------------------------------------------------------------------



CRenderPass_M2_Instanced::CRenderPass_M2_Instanced(IRenderDevice & RenderDevice, const std::shared_ptr<BuildRenderListPassTemplated<CM2_Base_Instance>>& List, std::shared_ptr<IScene> scene, bool OpaqueDraw)
	: CRenderPass_M2(RenderDevice, scene, OpaqueDraw)
	, m_RenderListPass(List)
{
	m_InstancesBuffer = GetRenderDevice().GetObjectsFactory().CreateStructuredBuffer(nullptr, 1000, sizeof(glm::mat4), CPUAccess::Write);
}

CRenderPass_M2_Instanced::~CRenderPass_M2_Instanced()
{
}

void CRenderPass_M2_Instanced::Render(RenderEventArgs & e)
{
	std::unordered_map<const IModel*, std::vector<const ISceneNode3D*>> modelPriorMap;

	for (const auto& it : m_RenderListPass->GetModelsList())
	{
		const auto& collider = it.Node->GetComponent<IColliderComponent3D>();

		const ICameraComponent3D* camera = GetRenderEventArgs().CameraForCulling;
		_ASSERT(camera != nullptr);

		if (collider->IsCulledByDistance2D(camera, m_ADT_MDX_Distance->Get()))
			continue;

		if (collider->IsCulledByFrustum(camera))
			continue;

		auto& resultIter = modelPriorMap.find(it.Model);
		if (resultIter == modelPriorMap.end())
		{
			modelPriorMap.insert(std::make_pair(it.Model, std::vector<const ISceneNode3D*>({ it.Node })));
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

		if (instances.size() > m_InstancesBuffer->GetElementCount())
			m_InstancesBuffer = GetRenderDevice().GetObjectsFactory().CreateStructuredBuffer(instances, CPUAccess::Write);
		else
			m_InstancesBuffer->Set(instances);

		m_ShaderInstancesBufferParameter->SetStructuredBuffer(m_InstancesBuffer);
		m_ShaderInstancesBufferParameter->Bind();
		{
			DoRenderM2Model(static_cast<const CM2_Base_Instance*>(*it.second.begin()), static_cast<const CM2_Skin*>(it.first), m_OpaqueDraw, instances.size());
		}
		m_ShaderInstancesBufferParameter->Unbind();
	}


	//Log::Info("Test = '%d'", modelPriorMap.size());

}

std::shared_ptr<IRenderPassPipelined> CRenderPass_M2_Instanced::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_D3D/M2.hlsl", "VS_main_Inst");
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
