#pragma once

class CRenderPass_ADT_MCNK : public BasePass
{
    typedef BasePass base;
public:
	CRenderPass_ADT_MCNK(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline);
	virtual ~CRenderPass_ADT_MCNK();

    // IVisitor
    bool Visit(std::shared_ptr<SceneNode3D> node) override final;

private:
	CGroupQuality&						m_QualitySettings;
};