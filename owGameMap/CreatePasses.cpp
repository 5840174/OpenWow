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
	IBlendState::BlendMode alphaBlending(true, false, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendOperation::Add, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha);
	IBlendState::BlendMode disableBlending;
	IDepthStencilState::DepthMode enableDepthWrites(true, IDepthStencilState::DepthWrite::Enable);
	IDepthStencilState::DepthMode disableDepthWrites(false, IDepthStencilState::DepthWrite::Disable);

	// PIPELINES
	std::shared_ptr<IPipelineState> WDLPipeline = device->CreatePipelineState();
	WDLPipeline->GetBlendState()->SetBlendMode(disableBlending);
	WDLPipeline->GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	WDLPipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	WDLPipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	WDLPipeline->SetRenderTarget(_renderTarget);
	WDLPipeline->GetRasterizerState()->SetViewport(viewport);
	technique->AddPass(std::make_shared<CRenderPass_Sky>(scene, WDLPipeline));
}

void AddWDLPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	IBlendState::BlendMode alphaBlending(true, false, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendOperation::Add, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha);
	IBlendState::BlendMode disableBlending;
	IDepthStencilState::DepthMode enableDepthWrites(true, IDepthStencilState::DepthWrite::Enable);
	IDepthStencilState::DepthMode disableDepthWrites(false, IDepthStencilState::DepthWrite::Disable);

	// PIPELINES
	std::shared_ptr<IPipelineState> WDLPipeline = device->CreatePipelineState();
	WDLPipeline->GetBlendState()->SetBlendMode(disableBlending);
	WDLPipeline->GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	WDLPipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	WDLPipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	WDLPipeline->SetRenderTarget(_renderTarget);
	WDLPipeline->GetRasterizerState()->SetViewport(viewport);
	technique->AddPass(std::make_shared<CRenderPass_WDL>(scene, WDLPipeline));

}

void AddMCNKPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
	IBlendState::BlendMode alphaBlending(true, false, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendOperation::Add, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha);
	IBlendState::BlendMode disableBlending;
	IDepthStencilState::DepthMode enableDepthWrites(true, IDepthStencilState::DepthWrite::Enable);
	IDepthStencilState::DepthMode disableDepthWrites(false, IDepthStencilState::DepthWrite::Disable);

	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;

	if (_RenderDevice->GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			IShader::ShaderType::VertexShader, "shaders_D3D/MapChunk.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
		);
		g_pPixelShader = _RenderDevice->CreateShader(
			IShader::ShaderType::PixelShader, "shaders_D3D/MapChunk.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
		);

	}
	else if (_RenderDevice->GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			IShader::ShaderType::VertexShader, "shaders_OGL/MapChunk.vs", IShader::ShaderMacros(), "", ""
		);
		g_pPixelShader = _RenderDevice->CreateShader(
			IShader::ShaderType::PixelShader, "shaders_OGL/MapChunk.ps", IShader::ShaderMacros(), "", ""
		);
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	std::shared_ptr<IPipelineState> ADTPipeline = device->CreatePipelineState();
	ADTPipeline->GetBlendState()->SetBlendMode(disableBlending);
	ADTPipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	ADTPipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	ADTPipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	ADTPipeline->SetRenderTarget(_renderTarget);
	ADTPipeline->GetRasterizerState()->SetViewport(viewport);
	ADTPipeline->SetShader(IShader::ShaderType::VertexShader, g_pVertexShader);
	ADTPipeline->SetShader(IShader::ShaderType::PixelShader, g_pPixelShader);

	technique->AddPass(std::make_shared<CRenderPass_ADT_MCNK>(scene, ADTPipeline));
}

void AddWMOPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene)
{
    IBlendState::BlendMode alphaBlending(true, false, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendOperation::Add, IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha);
    IBlendState::BlendMode disableBlending;
    IDepthStencilState::DepthMode enableDepthWrites(true, IDepthStencilState::DepthWrite::Enable);
    IDepthStencilState::DepthMode disableDepthWrites(false, IDepthStencilState::DepthWrite::Disable);

	// CreateShaders
	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;

	if (_RenderDevice->GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			IShader::ShaderType::VertexShader, "shaders_D3D/WMO.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
		);

		g_pPixelShader = _RenderDevice->CreateShader(
			IShader::ShaderType::PixelShader, "shaders_D3D/WMO.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
		);
	}
	else if (_RenderDevice->GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		g_pVertexShader = _RenderDevice->CreateShader(
			IShader::ShaderType::VertexShader, "shaders_OGL/WMO.vs", IShader::ShaderMacros(), "", ""
		);

		g_pPixelShader = _RenderDevice->CreateShader(
			IShader::ShaderType::PixelShader, "shaders_OGL/WMO.ps", IShader::ShaderMacros(), "", ""
		);
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

    // PIPELINES
    std::shared_ptr<IPipelineState> WMOPipeline = device->CreatePipelineState();
    WMOPipeline->GetBlendState()->SetBlendMode(disableBlending);
    WMOPipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
    WMOPipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
    WMOPipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
    WMOPipeline->SetRenderTarget(_renderTarget);
    WMOPipeline->GetRasterizerState()->SetViewport(viewport);
	WMOPipeline->SetShader(IShader::ShaderType::VertexShader, g_pVertexShader);
	WMOPipeline->SetShader(IShader::ShaderType::PixelShader, g_pPixelShader);
    technique->AddPass(std::make_shared<CRenderPass_WMO>(scene, WMOPipeline));
}