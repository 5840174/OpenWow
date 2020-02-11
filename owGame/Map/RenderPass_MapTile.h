#pragma once

class ZN_API CRenderPass_MapTile
	: public ScenePass
{
public:
	CRenderPass_MapTile(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_MapTile();

    // IVisitor
    bool Visit(const ISceneNode3D* node) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};