#pragma once

class CRenderPass_M2 : public Base3DPass
{
public:
	CRenderPass_M2(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline);
	virtual ~CRenderPass_M2();

    // IVisitor
    bool Visit(ISceneNode3D* node) override final;
	bool Visit(IMesh* Mesh, UINT IndexStartLocation = 0, UINT IndexCnt = 0, UINT VertexStartLocation = 0, UINT VertexCnt = 0) override final;
};