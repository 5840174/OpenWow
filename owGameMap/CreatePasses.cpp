#include "stdafx.h"

// General
#include "CreatePasses.h"

// Additional
#include "RenderPass_Sky.h"
#include "RenderPass_ADT_MCNK.h"
#include "RenderPass_WDL.h"
#include "WMO\\RenderPass_WMO.h"

void AddSkyPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	BlendState::BlendMode alphaBlending(true, false, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha, BlendState::BlendOperation::Add, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha);
	BlendState::BlendMode disableBlending;
	DepthStencilState::DepthMode enableDepthWrites(true, DepthStencilState::DepthWrite::Enable);
	DepthStencilState::DepthMode disableDepthWrites(false, DepthStencilState::DepthWrite::Disable);

	// PIPELINES
	std::shared_ptr<PipelineState> WDLPipeline = device->CreatePipelineState();
	WDLPipeline->GetBlendState().SetBlendMode(disableBlending);
	WDLPipeline->GetDepthStencilState().SetDepthMode(disableDepthWrites);
	WDLPipeline->GetRasterizerState().SetCullMode(RasterizerState::CullMode::None);
	WDLPipeline->GetRasterizerState().SetFillMode(RasterizerState::FillMode::Solid);
	WDLPipeline->SetRenderTarget(_renderTarget);
	WDLPipeline->GetRasterizerState().SetViewport(viewport);
	technique->AddPass(std::make_shared<CRenderPass_Sky>(scene, WDLPipeline));
}

void AddWDLPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	BlendState::BlendMode alphaBlending(true, false, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha, BlendState::BlendOperation::Add, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha);
	BlendState::BlendMode disableBlending;
	DepthStencilState::DepthMode enableDepthWrites(true, DepthStencilState::DepthWrite::Enable);
	DepthStencilState::DepthMode disableDepthWrites(false, DepthStencilState::DepthWrite::Disable);

	// PIPELINES
	std::shared_ptr<PipelineState> WDLPipeline = device->CreatePipelineState();
	WDLPipeline->GetBlendState().SetBlendMode(disableBlending);
	WDLPipeline->GetDepthStencilState().SetDepthMode(disableDepthWrites);
	WDLPipeline->GetRasterizerState().SetCullMode(RasterizerState::CullMode::Back);
	WDLPipeline->GetRasterizerState().SetFillMode(RasterizerState::FillMode::Solid);
	WDLPipeline->SetRenderTarget(_renderTarget);
	WDLPipeline->GetRasterizerState().SetViewport(viewport);
	technique->AddPass(std::make_shared<CRenderPass_WDL>(scene, WDLPipeline));

}

void AddMCNKPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	BlendState::BlendMode alphaBlending(true, false, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha, BlendState::BlendOperation::Add, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha);
	BlendState::BlendMode disableBlending;
	DepthStencilState::DepthMode enableDepthWrites(true, DepthStencilState::DepthWrite::Enable);
	DepthStencilState::DepthMode disableDepthWrites(false, DepthStencilState::DepthWrite::Disable);

	std::shared_ptr<Shader> g_pVertexShader;
	std::shared_ptr<Shader> g_pPixelShader;

	if (_RenderDevice->GetDeviceType() == IRenderDevice::DeviceType::DirectX)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			Shader::VertexShader, "shaders_D3D/MapChunk.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
		);
		g_pPixelShader = _RenderDevice->CreateShader(
			Shader::PixelShader, "shaders_D3D/MapChunk.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
		);

	}
	else if (_RenderDevice->GetDeviceType() == IRenderDevice::DeviceType::OpenGL)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			Shader::VertexShader, "shaders_OGL/MapChunk.vs", Shader::ShaderMacros(), "", ""
		);
		g_pPixelShader = _RenderDevice->CreateShader(
			Shader::PixelShader, "shaders_OGL/MapChunk.ps", Shader::ShaderMacros(), "", ""
		);
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	std::shared_ptr<PipelineState> ADTPipeline = device->CreatePipelineState();
	ADTPipeline->GetBlendState().SetBlendMode(disableBlending);
	ADTPipeline->GetDepthStencilState().SetDepthMode(enableDepthWrites);
	ADTPipeline->GetRasterizerState().SetCullMode(RasterizerState::CullMode::Back);
	ADTPipeline->GetRasterizerState().SetFillMode(RasterizerState::FillMode::Solid);
	ADTPipeline->SetRenderTarget(_renderTarget);
	ADTPipeline->GetRasterizerState().SetViewport(viewport);
	ADTPipeline->SetShader(Shader::ShaderType::VertexShader, g_pVertexShader);
	ADTPipeline->SetShader(Shader::ShaderType::PixelShader, g_pPixelShader);

	technique->AddPass(std::make_shared<CRenderPass_ADT_MCNK>(scene, ADTPipeline));
}

void AddWMOPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
    BlendState::BlendMode alphaBlending(true, false, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha, BlendState::BlendOperation::Add, BlendState::BlendFactor::SrcAlpha, BlendState::BlendFactor::OneMinusSrcAlpha);
    BlendState::BlendMode disableBlending;
    DepthStencilState::DepthMode enableDepthWrites(true, DepthStencilState::DepthWrite::Enable);
    DepthStencilState::DepthMode disableDepthWrites(false, DepthStencilState::DepthWrite::Disable);

	// CreateShaders
	std::shared_ptr<Shader> g_pVertexShader;
	std::shared_ptr<Shader> g_pPixelShader;

	if (_RenderDevice->GetDeviceType() == IRenderDevice::DeviceType::DirectX)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			Shader::VertexShader, "shaders_D3D/WMO.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
		);

		g_pPixelShader = _RenderDevice->CreateShader(
			Shader::PixelShader, "shaders_D3D/WMO.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
		);
	}
	else if (_RenderDevice->GetDeviceType() == IRenderDevice::DeviceType::OpenGL)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			Shader::VertexShader, "shaders_OGL/WMO.vs", Shader::ShaderMacros(), "", ""
		);

		g_pPixelShader = _RenderDevice->CreateShader(
			Shader::PixelShader, "shaders_OGL/WMO.ps", Shader::ShaderMacros(), "", ""
		);
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

    // PIPELINES
    std::shared_ptr<PipelineState> WMOPipeline = device->CreatePipelineState();
    WMOPipeline->GetBlendState().SetBlendMode(disableBlending);
    WMOPipeline->GetDepthStencilState().SetDepthMode(enableDepthWrites);
    WMOPipeline->GetRasterizerState().SetCullMode(RasterizerState::CullMode::Back);
    WMOPipeline->GetRasterizerState().SetFillMode(RasterizerState::FillMode::Solid);
    WMOPipeline->SetRenderTarget(_renderTarget);
    WMOPipeline->GetRasterizerState().SetViewport(viewport);
	WMOPipeline->SetShader(Shader::VertexShader, g_pVertexShader);
	WMOPipeline->SetShader(Shader::PixelShader, g_pPixelShader);
    technique->AddPass(std::make_shared<CRenderPass_WMO>(scene, WMOPipeline));
}