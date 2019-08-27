#pragma once

// A pass that renders the opaque geometry in the scene.
class CRenderPass_Liquid : public BasePass
{
public:
	typedef BasePass base;

	CRenderPass_Liquid(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline);
	virtual ~CRenderPass_Liquid();

    // IVisitor
    bool Visit(SceneNode3D* node) override final;
};