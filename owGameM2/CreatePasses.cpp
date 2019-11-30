#include "stdafx.h"

// General
#include "CreatePasses.h"

// Additional
#include "RenderPass_M2.h"

void AddM2Passes(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	// STATES
	IBlendState::BlendMode alphaBlending(true, false, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendOperation::Add, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha);
	IDepthStencilState::DepthMode enableDepthWrites(true, IDepthStencilState::DepthWrite::Enable);
	IDepthStencilState::DepthMode disableDepthWrites(false, IDepthStencilState::DepthWrite::Disable);


	// CreateShaders
	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;
	if (device->GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			IShader::ShaderType::VertexShader, "shaders_D3D/M2.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
		);
		g_pVertexShader->LoadInputLayoutFromReflector();

		g_pPixelShader = _RenderDevice->CreateShader(
			IShader::ShaderType::PixelShader, "shaders_D3D/M2.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
		);
	}
	else if (device->GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{

	}

	// PIPELINES
	std::shared_ptr<IPipelineState> M2Pipeline = device->CreatePipelineState();
	M2Pipeline->GetBlendState()->SetBlendMode(alphaBlending);
	M2Pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	M2Pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	M2Pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	M2Pipeline->SetRenderTarget(_renderTarget);
	M2Pipeline->GetRasterizerState()->SetViewport(viewport);
	M2Pipeline->SetShader(IShader::ShaderType::VertexShader, g_pVertexShader);
	M2Pipeline->SetShader(IShader::ShaderType::PixelShader, g_pPixelShader);

	technique->AddPass(std::make_shared<CRenderPass_M2>(scene, M2Pipeline));
}
