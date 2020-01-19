#include "stdafx.h"

// General
#include "RenderPass_ADT_MCNK.h"

// Additional
#include "MapChunk.h"

CRenderPass_ADT_MCNK::CRenderPass_ADT_MCNK(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
{
	m_WoWSettings = RenderDevice->GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_ADT_MCNK::~CRenderPass_ADT_MCNK()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_ADT_MCNK::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;

	if (GetRenderDevice()->GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = GetRenderDevice()->CreateShader(
			EShaderType::VertexShader, "shaders_D3D/MapChunk.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
		);
		g_pPixelShader = GetRenderDevice()->CreateShader(
			EShaderType::PixelShader, "shaders_D3D/MapChunk.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
		);
	}
	else if (GetRenderDevice()->GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		g_pVertexShader = GetRenderDevice()->CreateShader(
			EShaderType::VertexShader, "shaders_OGL/MapChunk.vs", IShader::ShaderMacros(), "", ""
		);
		g_pPixelShader = GetRenderDevice()->CreateShader(
			EShaderType::PixelShader, "shaders_OGL/MapChunk.ps", IShader::ShaderMacros(), "", ""
		);
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice()->CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(disableBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
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
bool CRenderPass_ADT_MCNK::Visit3D(ISceneNode* node)
{
    CMapChunk* adtMCNKInstance = dynamic_cast<CMapChunk*>(node);
    if (adtMCNKInstance == nullptr)
        return false;

    return Base3DPass::Visit3D(node);
}
