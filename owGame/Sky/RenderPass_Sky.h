#pragma once

class ZN_API CRenderPass_Sky 
	: public CBaseList3DPass
{
public:
	CRenderPass_Sky(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass);
	virtual ~CRenderPass_Sky();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    EVisitResult Visit(const ISceneNode* node) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
};