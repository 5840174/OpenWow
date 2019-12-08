#pragma once

class CRenderPass_WDL : public Base3DPass
{
public:
	CRenderPass_WDL(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline);
	virtual ~CRenderPass_WDL();

    // IVisitor
	bool Visit3D(ISceneNode* node) override final;
	bool Visit(IMesh* Mesh, UINT IndexStartLocation = 0, UINT IndexCnt = 0, UINT VertexStartLocation = 0, UINT VertexCnt = 0) override final;

private:
	std::shared_ptr<ISettingGroup> m_WoWSettings;
};