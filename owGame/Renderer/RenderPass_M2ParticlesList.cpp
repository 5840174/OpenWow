#include "stdafx.h"

// General
#include "RenderPass_M2ParticlesList.h"

// Additional
#include "M2/M2_Base_Instance.h"
#include "M2/M2_ParticlesComponent.h"

CRenderPass_M2ParticlesList::CRenderPass_M2ParticlesList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: CRenderPassPipelinedProcessTypelessList(RenderDevice, CreateTypelessList)
{
	m_Geometry = GetRenderDevice().GetObjectsFactory().CreateGeometry();
	m_Geometry->SetPrimitiveTopology(PrimitiveTopology::PointList);
}

CRenderPass_M2ParticlesList::~CRenderPass_M2ParticlesList()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_M2ParticlesList::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);
	
	{
		std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "3D/Particle.hlsl", "VS_main");
		vertexShader->LoadInputLayoutFromReflector();
		GetPipeline().SetShader(vertexShader);
	}

	{
		std::shared_ptr<IShader> geomShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::GeometryShader, "3D/Particle.hlsl", "GS_Billboard");
		
		m_GeomShaderParticlesBufferParameter = geomShader->GetShaderParameterByName("Particles");
		_ASSERT(m_GeomShaderParticlesBufferParameter);

		GetPipeline().SetShader(geomShader);
	}

	{
		std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "3D/Particle.hlsl", "PS_main");
		GetPipeline().SetShader(pixelShader);
	}

	// PIPELINES
	GetPipeline().GetBlendState()->SetBlendMode(//alphaBlending
		IBlendState::BlendMode(true, false,
		IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One,
		IBlendState::BlendOperation::Add,
		IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One)
	);
	GetPipeline().GetDepthStencilState()->SetDepthMode(enableTestDisableWrites);
	
	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_M2ParticlesList::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	if (nullptr == std::dynamic_pointer_cast<CM2_Base_Instance>(SceneNode))
		return EVisitResult::Block;

	if (false == SceneNode->IsComponentExistsT<IParticleComponent3D>())
		return EVisitResult::Block;

	return __super::Visit(SceneNode);
	
}

EVisitResult CRenderPass_M2ParticlesList::Visit(const std::shared_ptr<IParticleSystem>& ParticlesSystem)
{
	const auto& partilces = ParticlesSystem->GetGPUParticles();
	if (partilces.empty())
		return EVisitResult::Block;

	if (m_GeomParticlesBuffer == nullptr || partilces.size() != m_GeomParticlesBuffer->GetElementCount())
		m_GeomParticlesBuffer = GetRenderDevice().GetObjectsFactory().CreateStructuredBuffer(partilces, EAccess::CPUWrite);
	else
		m_GeomParticlesBuffer->Set(partilces);

	m_GeomShaderParticlesBufferParameter->SetStructuredBuffer(m_GeomParticlesBuffer);
	m_GeomShaderParticlesBufferParameter->Bind();

	const auto& shaders = GetPipeline().GetShaders();
	const auto& vertexShader = shaders.at(EShaderType::VertexShader).get();
	const auto& pixelShader = shaders.at(EShaderType::PixelShader).get();

	// Bind material
	if (ParticlesSystem->GetTexture())
		ParticlesSystem->GetTexture()->Bind(0, pixelShader, IShaderParameter::EType::Texture);

	// Bind BlendState
	if (auto m2ParticleSystem = std::dynamic_pointer_cast<CM2ParticleSystem>(ParticlesSystem))
		m2ParticleSystem->GetBlendState()->Bind();

	// Draw geom
	SGeometryDrawArgs args;
	args.VertexCnt = partilces.size();
	m_Geometry->Render(vertexShader, args);

	m_GeomShaderParticlesBufferParameter->Unbind();

	return EVisitResult::AllowAll;
}
