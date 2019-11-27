#include "stdafx.h"

// General
#include "CreatePasses.h"

// Additional
#include "RenderPass_M2.h"

void AddM2Passes(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	// STATES
	BlendState::BlendMode alphaBlending(true, false, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha, BlendState::BlendOperation::Add, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha);
	DepthStencilState::DepthMode enableDepthWrites(true, DepthStencilState::DepthWrite::Enable);
	DepthStencilState::DepthMode disableDepthWrites(false, DepthStencilState::DepthWrite::Disable);


	// CreateShaders
	std::shared_ptr<Shader> g_pVertexShader = _RenderDevice->CreateShader(
		Shader::VertexShader, "shaders_D3D/M2.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
	);
	g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<Shader> g_pPixelShader = _RenderDevice->CreateShader(
		Shader::PixelShader, "shaders_D3D/M2.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
	);

	// PIPELINES
	std::shared_ptr<PipelineState> M2Pipeline = device->CreatePipelineState();
	M2Pipeline->GetBlendState().SetBlendMode(alphaBlending);
	M2Pipeline->GetDepthStencilState().SetDepthMode(enableDepthWrites);
	M2Pipeline->GetRasterizerState().SetCullMode(RasterizerState::CullMode::None);
	M2Pipeline->GetRasterizerState().SetFillMode(RasterizerState::FillMode::Solid);
	M2Pipeline->SetRenderTarget(_renderTarget);
	M2Pipeline->GetRasterizerState().SetViewport(viewport);
	M2Pipeline->SetShader(Shader::VertexShader, g_pVertexShader);
	M2Pipeline->SetShader(Shader::PixelShader, g_pPixelShader);

	technique->AddPass(std::make_shared<CRenderPass_M2>(scene, M2Pipeline));
}
