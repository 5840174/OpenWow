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
	pipeline->SetShader(EShaderType::VertexShader, vertexShader);
	pipeline->SetShader(EShaderType::PixelShader, pixelShader);

	return SetPipeline(pipeline);
}



//
// IVisitor
//
EVisitResult CRenderPass_WDL::Visit(const ISceneNode3D* SceneNode)
{
	if (SceneNode->Is(cMap_NodeType))
	{
		//const CMap* map = static_cast<const CMap*>(SceneNode);
		//if (map == nullptr)
		//	return false;

		return Base3DPass::Visit(SceneNode);
	}

	return EVisitResult::AllowVisitChilds;
}

EVisitResult CRenderPass_WDL::Visit(const IModel* Model)
{
	const CWDL_LowResTile* wdlMesh = static_cast<const CWDL_LowResTile*>(Model);
	if (wdlMesh == nullptr)
        return EVisitResult::Block;

	if (! wdlMesh->IsNeedRender())
		return EVisitResult::Block;

	Model->Render(GetRenderEventArgs());
	return EVisitResult::AllowAll;
}
