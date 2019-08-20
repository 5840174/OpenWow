#include "stdafx.h"

// General
#include "CreatePasses.h"

// Additional
#include "RenderPass_Liquid.h"

void AddLiquidPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	// STATES
	BlendState::BlendMode alphaBlending(true, false, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha, BlendState::BlendOperation::Add, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha);
	DepthStencilState::DepthMode enableDepthWrites(true, DepthStencilState::DepthWrite::Enable);
	DepthStencilState::DepthMode disableDepthWrites(false, DepthStencilState::DepthWrite::Disable);

	// PIPELINES
	std::shared_ptr<PipelineState> WMOPipeline = device->CreatePipelineState();
	WMOPipeline->GetBlendState().SetBlendMode(alphaBlending);
	WMOPipeline->GetDepthStencilState().SetDepthMode(enableDepthWrites);
	WMOPipeline->GetRasterizerState().SetCullMode(RasterizerState::CullMode::None);
	WMOPipeline->GetRasterizerState().SetFillMode(RasterizerState::FillMode::Solid);
	WMOPipeline->SetRenderTarget(_renderTarget);
	WMOPipeline->GetRasterizerState().SetViewport(viewport);

	technique->AddPass(std::make_shared<CRenderPass_Liquid>(scene, WMOPipeline));
}
