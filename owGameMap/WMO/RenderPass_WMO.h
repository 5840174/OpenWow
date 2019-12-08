#pragma once

class CRenderPass_WMO : public Base3DPass
{
public:
	CRenderPass_WMO(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline);
	virtual ~CRenderPass_WMO();

    // IVisitor
    bool Visit3D(ISceneNode* node) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};