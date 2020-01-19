#pragma once

class CRenderPass_Sky : public Base3DPass
{
public:
	CRenderPass_Sky(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_Sky();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    bool Visit3D(ISceneNode* node) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};