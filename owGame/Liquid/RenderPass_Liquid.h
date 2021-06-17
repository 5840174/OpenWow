#pragma once

class ZN_API CRenderPass_Liquid
	: public CBaseList3DPass
{
public:
	CRenderPass_Liquid(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass);
	virtual ~CRenderPass_Liquid();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    EVisitResult Visit(const ISceneNode* node) override final;
};