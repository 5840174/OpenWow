#pragma once

#include "RenderPass_M2List.h"

class ZN_API CRenderPass_M2InstancedList
	: public CRenderPass_M2List
{
public:
	CRenderPass_M2InstancedList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList, ERenderPassM2DrawMode DrawMode = ERenderPassM2DrawMode::All);
	virtual ~CRenderPass_M2InstancedList();

	// IRenderPass
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override;

	// IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& SceneNode) override final;
	EVisitResult Visit(const std::shared_ptr<IModel>& Model) override final;

private:
	IShaderParameter*                  m_ShaderInstancesBufferParameter;
	std::shared_ptr<IStructuredBuffer> m_InstancesBuffer;

	const CM2_Base_Instance* m_CurrentM2Instance;
	std::unordered_map<const CM2_Skin*, std::vector<const CM2_Base_Instance*>> m_ModelsNodes;
};
