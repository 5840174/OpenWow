#include "stdafx.h"

// General
#include "RenderPass_Sky.h"

// Additional 
#include "SkyManager.h"

CRenderPass_Sky::CRenderPass_Sky(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
{
	m_WoWSettings = RenderDevice->GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_Sky::~CRenderPass_Sky()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_Sky::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;

	if (GetRenderDevice()->GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = GetRenderDevice()->CreateShader(
			EShaderType::VertexShader, "shaders_D3D/Sky.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
		);
		g_pPixelShader = GetRenderDevice()->CreateShader(
			EShaderType::PixelShader, "shaders_D3D/Sky.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
		);
	}
	else if (GetRenderDevice()->GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		g_pVertexShader = GetRenderDevice()->CreateShader(
			EShaderType::VertexShader, "shaders_OGL/Sky.vs", IShader::ShaderMacros(), "", ""
		);
		g_pPixelShader = GetRenderDevice()->CreateShader(
			EShaderType::PixelShader, "shaders_OGL/Sky.ps", IShader::ShaderMacros(), "", ""
		);
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice()->CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(disableBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->GetRasterizerState()->SetViewport(Viewport);
	pipeline->SetShader(EShaderType::VertexShader, g_pVertexShader);
	pipeline->SetShader(EShaderType::PixelShader, g_pPixelShader);

	return SetPipeline(pipeline);
}



//
// IVisitor
//
bool CRenderPass_Sky::Visit3D(ISceneNode* node)
{
    SkyManager* skyManagerInstance = dynamic_cast<SkyManager*>(node);
    if (skyManagerInstance == nullptr)
        return false;

    return Base3DPass::Visit3D(node);
}
