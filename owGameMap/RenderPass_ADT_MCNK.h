#pragma once

class CRenderPass_ADT_MCNK : public Base3DPass
{
public:
	CRenderPass_ADT_MCNK(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline);
	virtual ~CRenderPass_ADT_MCNK();

    // IVisitor
    bool Visit(ISceneNode3D* node) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};