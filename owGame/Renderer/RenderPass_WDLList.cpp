#include "stdafx.h"

// General
#include "RenderPass_WDLList.h"

// Additional
#include "Map/MapWDLTileModel.h"
#include "Map/Map.h"

CRenderPass_WDLList::CRenderPass_WDLList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: CRenderPassProcessTypelessList(RenderDevice, CreateTypelessList)
{
	SetPassName("MapWDLList");

	m_WoWSettings = GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WDLList::~CRenderPass_WDLList()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_WDLList::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/MapWDL.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/MapWDL.hlsl", "PS_main");

	// PIPELINES
	GetPipeline().GetBlendState()->SetBlendMode(disableBlending);
	GetPipeline().GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Wireframe, IRasterizerState::FillMode::Wireframe);
	GetPipeline().SetRenderTarget(RenderTarget);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_WDLList::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	if (const auto map = std::dynamic_pointer_cast<CMap>(SceneNode))
		return __super::Visit(SceneNode);

	return EVisitResult::Block;
}

EVisitResult CRenderPass_WDLList::Visit(const std::shared_ptr<IModel>& Model)
{
	const auto wdlMesh = std::dynamic_pointer_cast<CMapWDLTileModel>(Model);
	if (wdlMesh == nullptr)
        return EVisitResult::Block;

	if (false == wdlMesh->IsNeedRender())
		return EVisitResult::Block;

	return __super::Visit(wdlMesh);
}
