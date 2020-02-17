#include "stdafx.h"

// General
#include "RenderPass_Liquid.h"

// Additional
#include "Liquid/LiquidInstance.h"

CRenderPass_Liquid::CRenderPass_Liquid(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene)
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
    if (const Liquid_Instance* liquidInstance = dynamic_cast<const Liquid_Instance*>(SceneNode3D))
    {
		const std::shared_ptr<IColliderComponent3D>& collider = SceneNode3D->GetComponent<IColliderComponent3D>();

		BoundingBox bbox = collider->GetBounds();
		bbox.transform(SceneNode3D->GetWorldTransfom());

		const ICameraComponent3D* camera = GetRenderEventArgs().CameraForCulling;
		_ASSERT(camera != nullptr);

		if (camera->GetFrustum().cullBox(bbox))
			return false;

        return Base3DPass::Visit(SceneNode3D);
    }

    return false;
}
