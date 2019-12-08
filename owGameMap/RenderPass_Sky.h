#pragma once

class CRenderPass_Sky : public Base3DPass
{
public:
	CRenderPass_Sky(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline);
	virtual ~CRenderPass_Sky();

    // IVisitor
    bool Visit(ISceneNode3D* node) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};