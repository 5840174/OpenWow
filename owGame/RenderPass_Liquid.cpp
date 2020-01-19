#include "stdafx.h"

// General
#include "RenderPass_Liquid.h"

// Additional
#include "LiquidInstance.h"

CRenderPass_Liquid::CRenderPass_Liquid(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
{}

CRenderPass_Liquid::~CRenderPass_Liquid()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_Liquid::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> g_pVertexShader = GetRenderDevice()->CreateShader(
		EShaderType::VertexShader, "shaders_D3D/Liquid.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
	);
	g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> g_pPixelShader = GetRenderDevice()->CreateShader(
		EShaderType::PixelShader, "shaders_D3D/Liquid.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
	);

	// PIPELINES
	std::shared_ptr<IPipelineState> WMOPipeline = GetRenderDevice()->CreatePipelineState();
	WMOPipeline->GetBlendState()->SetBlendMode(alphaBlending);
	WMOPipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	WMOPipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	WMOPipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	WMOPipeline->SetRenderTarget(RenderTarget);
	WMOPipeline->GetRasterizerState()->SetViewport(Viewport);
	WMOPipeline->SetShader(EShaderType::VertexShader, g_pVertexShader);
	WMOPipeline->SetShader(EShaderType::PixelShader, g_pPixelShader);

	return SetPipeline(WMOPipeline);
}



//
// IVisitor
//
bool CRenderPass_Liquid::Visit3D(ISceneNode* node)
{
    Liquid_Instance* liquidInstance = dynamic_cast<Liquid_Instance*>(node);
    if (liquidInstance)
    {
        return Base3DPass::Visit3D(node);
    }

    return false;
}
