#pragma once

#include "M2/M2_Base_Instance.h"

#include "RenderPass_M2.h"

class ZN_API CRenderPass_M2_Instanced
	: public CRenderPass_M2
{
public:
	CRenderPass_M2_Instanced(IScene& Scene, ERenderPassM2DrawMode DrawMode = ERenderPassM2DrawMode::All);
	virtual ~CRenderPass_M2_Instanced();

	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	virtual std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override;

	// IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;
	EVisitResult Visit(const std::shared_ptr<IModel>& Model) override final;

private:
	IShaderParameter*                  m_ShaderInstancesBufferParameter;
	std::shared_ptr<IStructuredBuffer> m_InstancesBuffer;

	const CM2_Base_Instance* m_CurrentM2Instance;
	std::unordered_map<const CM2_Skin*, std::vector<const CM2_Base_Instance*>> m_modelPriorMap;
};
