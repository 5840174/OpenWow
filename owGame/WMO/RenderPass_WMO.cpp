#include "stdafx.h"

// General
#include "RenderPass_WMO.h"

// Additional
#include "WMO_Base_Instance.h"
#include "WMO_Group_Instance.h"

CRenderPass_WMO::CRenderPass_WMO(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
{
	m_WoWSettings = RenderDevice->GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WMO::~CRenderPass_WMO()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_WMO::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;

	if (GetRenderDevice()->GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = GetRenderDevice()->CreateShader(
			EShaderType::VertexShader, "shaders_D3D/WMO.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
		);

		g_pPixelShader = GetRenderDevice()->CreateShader(
			EShaderType::PixelShader, "shaders_D3D/WMO.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
		);
	}
	else if (GetRenderDevice()->GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		g_pVertexShader = GetRenderDevice()->CreateShader(
			EShaderType::VertexShader, "shaders_OGL/WMO.vs", IShader::ShaderMacros(), "", ""
		);

		g_pPixelShader = GetRenderDevice()->CreateShader(
			EShaderType::PixelShader, "shaders_OGL/WMO.ps", IShader::ShaderMacros(), "", ""
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
bool CRenderPass_WMO::Visit3D(ISceneNode* node)
{
    CWMO_Base_Instance* wmoInstance = dynamic_cast<CWMO_Base_Instance*>(node);
	CWMO_Group_Instance* wmoGroupInstance = dynamic_cast<CWMO_Group_Instance*>(node);

    if (!wmoInstance && !wmoGroupInstance)
        return false;

    return Base3DPass::Visit3D(node);
}
