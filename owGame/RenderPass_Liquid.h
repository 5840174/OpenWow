#pragma once

class CRenderPass_Liquid : public Base3DPass
{
public:
	CRenderPass_Liquid(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_Liquid();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    bool Visit3D(ISceneNode* node) override final;
};