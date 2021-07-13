#pragma once

#include "RenderPassPipelinedProcessTypelessList.h"

#include "Sky/SkyManager.h"

class ZN_API CRenderPass_WDLList
	: public CRenderPassPipelinedProcessTypelessList
{
public:
	CRenderPass_WDLList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPass_WDLList();

	// IRenderPass
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
	EVisitResult Visit(const std::shared_ptr<ISceneNode>& SceneNode) override final;
	EVisitResult Visit(const std::shared_ptr<IModel>& Model) override final;

private:
	const CSkyManager* m_SkyManager;
	std::shared_ptr<IConstantBuffer> m_ColorConstantBuffer;
	IShaderParameter* m_ShaderColorParameter;
};