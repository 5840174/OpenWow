#include "stdafx.h"

// General
#include "RenderPass_WDL.h"

// Additional
#include "WDL_LowResTile.h"
#include "Map.h"

CRenderPass_WDL::CRenderPass_WDL(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
{
	m_WoWSettings = RenderDevice.GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WDL::~CRenderPass_WDL()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_WDL::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_D3D/MapWDL.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "shaders_D3D/MapWDL.hlsl", "PS_main");

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(disableBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Wireframe);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->GetRasterizerState()->SetViewport(Viewport);
	pipeline->SetShader(EShaderType::VertexShader, vertexShader);
	pipeline->SetShader(EShaderType::PixelShader, pixelShader);

	return SetPipeline(pipeline);
}



//
// IVisitor
//
bool CRenderPass_WDL::Visit(const ISceneNode3D* node)
{
	const CMap* map = dynamic_cast<const CMap*>(node);
    if (map == nullptr)
        return false;

    return Base3DPass::Visit(node);
}

bool CRenderPass_WDL::Visit(const IModel* Mesh)
{
	const CWDL_LowResTile* wdlMesh = dynamic_cast<const CWDL_LowResTile*>(Mesh);
	if (wdlMesh == nullptr)
        return false;

	return Mesh->Render(GetRenderEventArgs());
}
