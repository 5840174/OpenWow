#include "stdafx.h"

// General
#include "CreatePasses.h"

// Additional
#include "RenderPass_Liquid.h"

void AddLiquidPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	// STATES
	IBlendState::BlendMode alphaBlending(true, false, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendOperation::Add, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha);
	IDepthStencilState::DepthMode enableDepthWrites(true, IDepthStencilState::DepthWrite::Enable);
	IDepthStencilState::DepthMode disableDepthWrites(false, IDepthStencilState::DepthWrite::Disable);

	// PIPELINES
	std::shared_ptr<IPipelineState> WMOPipeline = device->CreatePipelineState();
	WMOPipeline->GetBlendState()->SetBlendMode(alphaBlending);
	WMOPipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	WMOPipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	WMOPipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	WMOPipeline->SetRenderTarget(_renderTarget);
	WMOPipeline->GetRasterizerState()->SetViewport(viewport);

	technique->AddPass(std::make_shared<CRenderPass_Liquid>(scene, WMOPipeline));
}
