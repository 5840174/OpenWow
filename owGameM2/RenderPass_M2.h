#pragma once

class ZN_API CRenderPass_M2 
	: public Base3DPass
{
public:
	CRenderPass_M2(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene);
	virtual ~CRenderPass_M2();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport* Viewport) override final;

    // IVisitor
    bool Visit3D(ISceneNode* node) override final;
	bool Visit(IMesh* Mesh, SGeometryPartParams GeometryPartParams = SGeometryPartParams()) override final;
};