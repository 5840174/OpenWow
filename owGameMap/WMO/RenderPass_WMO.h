#pragma once

class CRenderPass_WMO : public BasePass
{
    typedef BasePass base;
public:
	CRenderPass_WMO(std::shared_ptr<Scene3D> scene, std::shared_ptr<IPipelineState> pipeline);
	virtual ~CRenderPass_WMO();

    // IVisitor
    bool Visit(SceneNode3D* node) override final;
};