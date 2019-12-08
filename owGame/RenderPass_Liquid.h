#pragma once

class CRenderPass_Liquid : public Base3DPass
{
public:
	CRenderPass_Liquid(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline);
	virtual ~CRenderPass_Liquid();

    // IVisitor
    bool Visit3D(ISceneNode* node) override final;
};