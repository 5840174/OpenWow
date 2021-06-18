#pragma once

class ZN_API CRenderPass_Sky 
	: public Base3DPass
{
public:
	CRenderPass_Sky(IScene& Scene);
	virtual ~CRenderPass_Sky();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr < ISceneNode>& node) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
};