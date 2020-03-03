#pragma once

class ZN_API CRenderPass_Liquid
	: public CBaseList3DPass
{
public:
	CRenderPass_Liquid(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneNodeListPass>& SceneNodeListPass);
	virtual ~CRenderPass_Liquid();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    bool Visit(const ISceneNode3D* node) override final;
};