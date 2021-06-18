#pragma once

class ZN_API CRenderPass_ADT_MCNK 
	: public Base3DPass
{
public:
	CRenderPass_ADT_MCNK(IScene& Scene);
	virtual ~CRenderPass_ADT_MCNK();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;

private:
	std::shared_ptr<IPropertyT<float>> m_ADT_MCNK_Distance;
};