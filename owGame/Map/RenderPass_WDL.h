#pragma once

class ZN_API CRenderPass_WDL
	: public Base3DPass
{
public:
	CRenderPass_WDL(IScene& Scene);
	virtual ~CRenderPass_WDL();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& SceneNode) override final;
	EVisitResult Visit(const std::shared_ptr<IModel>& Model) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
};