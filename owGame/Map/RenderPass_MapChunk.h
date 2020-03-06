#pragma once

class ZN_API CRenderPass_ADT_MCNK 
	: public CBaseList3DPass
{
public:
	CRenderPass_ADT_MCNK(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass);
	virtual ~CRenderPass_ADT_MCNK();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    EVisitResult Visit(const ISceneNode3D* node) override final;

private:
	std::shared_ptr<ISettingT<float>> m_ADT_MCNK_Distance;
};