#include "stdafx.h"

// General
#include "RenderPass_Liquid.h"

// Additional
#include "Liquid/LiquidInstance.h"

CRenderPass_Liquid::CRenderPass_Liquid(IScene& Scene)
	: Base3DPass(Scene)
{}

CRenderPass_Liquid::~CRenderPass_Liquid()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_Liquid::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/Liquid.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/Liquid.hlsl", "PS_main");

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(alphaBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->SetShader(vertexShader);
	pipeline->SetShader(pixelShader);

	// Create samplers
	std::shared_ptr<ISamplerState> sampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	sampler->SetWrapMode(ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp);
	pipeline->SetSampler(0, sampler);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_Liquid::Visit(const std::shared_ptr<ISceneNode>& SceneNode3D)
{
	//_ASSERT(SceneNode3D->Is(cLiquid_NodeType) || SceneNode3D->Is(cLiquid_MapChnuk_NodeType) || SceneNode3D->Is(cLiquid_WMOGroup_NodeType));
	
	if (auto liquidInstance = std::dynamic_pointer_cast<Liquid_Instance>(SceneNode3D))
	{
		return __super::Visit(liquidInstance);
	}

	_ASSERT(false);
    return EVisitResult::Block;
}
