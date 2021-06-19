#pragma once

class ZN_API CRenderPass_MapTile
	: public Base3DPass
{
public:
	CRenderPass_MapTile(IScene& Scene, const std::shared_ptr<IRenderPassCreateTypelessList>& SceneListTypeless);
	virtual ~CRenderPass_MapTile();

    // IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
	std::shared_ptr<IRenderPassCreateTypelessList> m_SceneListTypeless;
};