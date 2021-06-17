#pragma once

class ZN_API CRenderPass_MapTile
	: public ScenePass
{
public:
	CRenderPass_MapTile(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_MapTile();

    // IVisitor
	EVisitResult Visit(const ISceneNode* node) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
};