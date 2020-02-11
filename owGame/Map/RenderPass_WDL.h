#pragma once

class CRenderPass_WDL 
	: public Base3DPass
{
public:
	CRenderPass_WDL(IRenderDevice& RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_WDL();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
	bool Visit(const ISceneNode3D* node) override final;
	bool Visit(const IModel* Mesh) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};