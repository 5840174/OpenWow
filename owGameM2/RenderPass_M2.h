#pragma once

class CRenderPass_M2 : public BasePass
{
    typedef BasePass base;
public:
	CRenderPass_M2(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline);
	virtual ~CRenderPass_M2();

    // IVisitor
    bool Visit(SceneNode3D* node) override final;
	bool Visit(IMesh* Mesh, UINT IndexStartLocation = 0, UINT IndexCnt = 0, UINT VertexStartLocation = 0, UINT VertexCnt = 0) override final;
};