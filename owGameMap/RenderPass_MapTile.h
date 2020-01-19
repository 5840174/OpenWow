#pragma once

class CRenderPass_MapTile 
	: public ScenePass
{
public:
	CRenderPass_MapTile(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_MapTile();

    // IVisitor
    bool Visit3D(ISceneNode* node) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};