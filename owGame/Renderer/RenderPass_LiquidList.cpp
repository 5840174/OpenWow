#include "stdafx.h"

// General
#include "RenderPass_LiquidList.h"

// Additional
#include "Liquid/LiquidBaseInstance.h"

CRenderPass_LiquidList::CRenderPass_LiquidList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: CRenderPassProcessTypelessList(RenderDevice, CreateTypelessList)
{
	SetPassName("LiquidList");
}

CRenderPass_LiquidList::~CRenderPass_LiquidList()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_LiquidList::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/Liquid.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/Liquid.hlsl", "PS_main");

	GetPipeline().GetBlendState()->SetBlendMode(alphaBlending);
	GetPipeline().GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	GetPipeline().SetRenderTarget(RenderTarget);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_LiquidList::Visit(const std::shared_ptr<ISceneNode>& SceneNode3D)
{
	if (auto liquidInstance = std::dynamic_pointer_cast<CLiquidBaseInstance>(SceneNode3D))
		return __super::Visit(liquidInstance);

    return EVisitResult::Block;
}
