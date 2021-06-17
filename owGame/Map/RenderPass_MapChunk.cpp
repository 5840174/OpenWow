#include "stdafx.h"

// General
#include "RenderPass_MapChunk.h"

// Additional
#include "MapChunk.h"

CRenderPass_ADT_MCNK::CRenderPass_ADT_MCNK(IScene& Scene)
	: Base3DPass(Scene)
{
	m_ADT_MCNK_Distance = GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("ADT_MCNK_Distance");
}

CRenderPass_ADT_MCNK::~CRenderPass_ADT_MCNK()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_ADT_MCNK::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	std::shared_ptr<IShader> vertexShader;
	std::shared_ptr<IShader> pixelShader;

	if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX11)
	{
		vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/MapChunk.hlsl", "VS_main");
		pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/MapChunk.hlsl", "PS_main");
	}
	else if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_OGL/MapChunk.vs", "");
		pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_OGL/MapChunk.ps", "");
	}
	vertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(disableBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->SetShader(vertexShader);
	pipeline->SetShader(pixelShader);

	// Create samplers
	std::shared_ptr<ISamplerState> linearClampSampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	linearClampSampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipNearest);
	linearClampSampler->SetWrapMode(ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp);

	std::shared_ptr<ISamplerState> linearRepeatSampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	linearRepeatSampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	linearRepeatSampler->SetWrapMode(ISamplerState::WrapMode::Repeat, ISamplerState::WrapMode::Repeat, ISamplerState::WrapMode::Repeat);

	pipeline->SetSampler(0, linearRepeatSampler);
	pipeline->SetSampler(1, linearClampSampler);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_ADT_MCNK::Visit(const std::shared_ptr<ISceneNode>& SceneNode3D)
{
	if (const auto adtMCNKInstance = std::dynamic_pointer_cast<CMapChunk>(SceneNode3D))
	{
		return __super::Visit(adtMCNKInstance);
	}

	_ASSERT(false);
	return EVisitResult::Block;
}
