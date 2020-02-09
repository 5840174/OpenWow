#pragma once

class ZN_API CRenderPass_Sky : public Base3DPass
{
public:
	CRenderPass_Sky(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_Sky();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    bool Visit(const ISceneNode3D* node) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};