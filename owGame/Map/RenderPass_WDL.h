#pragma once

class ZN_API CRenderPass_WDL
	: public Base3DPass
{
public:
	CRenderPass_WDL(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_WDL();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
	EVisitResult Visit(const ISceneNode* SceneNode) override final;
	EVisitResult Visit(const IModel* Model) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
};