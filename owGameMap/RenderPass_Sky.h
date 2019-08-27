#pragma once

class CRenderPass_Sky : public BasePass
{
    typedef BasePass base;
public:
	CRenderPass_Sky(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline);
	virtual ~CRenderPass_Sky();

    // IVisitor
    bool Visit(SceneNode3D* node) override final;
};