#include "stdafx.h"

// General
#include "RenderPass_MapChunk.h"

// Additional
#include "MapChunk.h"

CRenderPass_ADT_MCNK::CRenderPass_ADT_MCNK(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass)
	: CBaseList3DPass(RenderDevice, SceneNodeListPass, cMapChunk_NodeType)
{
	m_ADT_MCNK_Distance = RenderDevice.GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetSettingT<float>("ADT_MCNK_Distance");
}

CRenderPass_ADT_MCNK::~CRenderPass_ADT_MCNK()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_ADT_MCNK::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	std::shared_ptr<IShader> vertexShader;
	std::shared_ptr<IShader> pixelShader;

	if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
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
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->SetShader(EShaderType::VertexShader, vertexShader);
	pipeline->SetShader(EShaderType::PixelShader, pixelShader);

	// Create samplers
	std::shared_ptr<ISamplerState> linearClampSampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	linearClampSampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipNearest);
	linearClampSampler->SetWrapMode(ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp);

	std::shared_ptr<ISamplerState> linearRepeatSampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	linearRepeatSampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	linearRepeatSampler->SetWrapMode(ISamplerState::WrapMode::Repeat, ISamplerState::WrapMode::Repeat, ISamplerState::WrapMode::Repeat);

	pipeline->SetSampler(0, linearRepeatSampler);
	pipeline->SetSampler(1, linearClampSampler);

	return SetPipeline(pipeline);
}



//
// IVisitor
//
EVisitResult CRenderPass_ADT_MCNK::Visit(const ISceneNode3D* SceneNode3D)
{
	_ASSERT(SceneNode3D->Is(cMapChunk_NodeType));
	
	if (const CMapChunk* adtMCNKInstance = static_cast<const CMapChunk*>(SceneNode3D))
	{
		return CBaseList3DPass::Visit(SceneNode3D);
	}

	_ASSERT(false);
	return EVisitResult::Block;
}
