#include "stdafx.h"

// General
#include "RenderPass_Liquid.h"

// Additional
#include "Liquid/LiquidInstance.h"

CRenderPass_Liquid::CRenderPass_Liquid(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneNodeListPass>& SceneNodeListPass)
	: CBaseList3DPass(RenderDevice, SceneNodeListPass, { cLiquid_NodeType, cLiquid_MapChnuk_NodeType, cLiquid_WMOGroup_NodeType } )
{}

CRenderPass_Liquid::~CRenderPass_Liquid()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_Liquid::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_D3D/Liquid.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "shaders_D3D/Liquid.hlsl", "PS_main");

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(alphaBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->GetRasterizerState()->SetViewport(Viewport);
	pipeline->SetShader(EShaderType::VertexShader, vertexShader);
	pipeline->SetShader(EShaderType::PixelShader, pixelShader);

	// Create samplers
	std::shared_ptr<ISamplerState> sampler = GetRenderDevice().GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	sampler->SetWrapMode(ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp);
	pipeline->SetSampler(0, sampler);

	return SetPipeline(pipeline);
}



//
// IVisitor
//
bool CRenderPass_Liquid::Visit(const ISceneNode3D* SceneNode3D)
{
	_ASSERT(SceneNode3D->Is(cLiquid_NodeType) || SceneNode3D->Is(cLiquid_MapChnuk_NodeType) || SceneNode3D->Is(cLiquid_WMOGroup_NodeType));
	
	if (const Liquid_Instance* liquidInstance = static_cast<const Liquid_Instance*>(SceneNode3D))
	{
		const std::shared_ptr<IColliderComponent3D>& collider = SceneNode3D->GetComponent<IColliderComponent3D>();

		const ICameraComponent3D* camera = GetRenderEventArgs().CameraForCulling;
		_ASSERT(camera != nullptr);

		if (camera->GetFrustum().cullBox(collider->GetWorldBounds()))
			return false;

		return CBaseList3DPass::Visit(SceneNode3D);
	}

	_ASSERT(false);
    return false;
}
