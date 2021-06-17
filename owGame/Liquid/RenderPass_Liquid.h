#pragma once

class ZN_API CRenderPass_Liquid
	: public Base3DPass
{
public:
	CRenderPass_Liquid(IScene& Scene);
	virtual ~CRenderPass_Liquid();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;
};