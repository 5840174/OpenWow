#pragma once

#include "RenderPassPipelinedProcessTypelessList.h"

#include "Sky/SkyManager.h"

class ZN_API CRenderPass_LiquidList
	: public CRenderPassPipelinedProcessTypelessList
{
public:
	CRenderPass_LiquidList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPass_LiquidList();
	
	// IRenderPass
	void Render(RenderEventArgs& e) override;

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;

private:
	const CSkyManager* m_SkyManager;
	std::shared_ptr<IConstantBuffer> m_LiquidColorsConstantBuffer;
	IShaderParameter* m_ShaderLiquidColorsParameter;
};
