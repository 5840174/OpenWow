#include "stdafx.h"

// General
#include "RenderPass_WDL.h"

// Additional
#include "WDL_LowResTile.h"
#include "Map.h"

CRenderPass_WDL::CRenderPass_WDL(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
{
	m_WoWSettings = RenderDevice->GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WDL::~CRenderPass_WDL()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_WDL::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> g_pVertexShader = GetRenderDevice()->CreateShader(
		EShaderType::VertexShader, "shaders_D3D/MapWDL.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
	);
	g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> g_pPixelShader = GetRenderDevice()->CreateShader(
		EShaderType::PixelShader, "shaders_D3D/MapWDL.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
	);

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice()->CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(disableBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(disableDepthWrites);
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
bool CRenderPass_WDL::Visit3D(ISceneNode* node)
{
    CMap* map = dynamic_cast<CMap*>(node);
    if (map == nullptr)
        return false;

    return Base3DPass::Visit3D(node);
}

bool CRenderPass_WDL::Visit(IModel* Mesh, SGeometryPartParams GeometryPartParams)
{
    CWDL_LowResTile* wdlMesh = dynamic_cast<CWDL_LowResTile*>(Mesh);
	if (wdlMesh == nullptr)
        return false;

	return Mesh->Render(GetRenderEventArgs(), m_PerObjectConstantBuffer.get(), GeometryPartParams);
}
