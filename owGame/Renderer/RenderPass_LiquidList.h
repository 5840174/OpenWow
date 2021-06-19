#pragma once

#include "RenderPassProcessTypelessList.h"

class ZN_API CRenderPass_LiquidList
	: public CRenderPassProcessTypelessList
{
public:
	CRenderPass_LiquidList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPass_LiquidList();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;
};
