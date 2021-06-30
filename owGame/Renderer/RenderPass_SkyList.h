#pragma once

#include "RenderPassPipelinedProcessTypelessList.h"

class ZN_API CRenderPass_SkyList
	: public CRenderPassPipelinedProcessTypelessList
{
public:
	CRenderPass_SkyList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPass_SkyList();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr < ISceneNode>& node) override final;

private:
	std::shared_ptr<IPropertiesGroup> m_WoWSettings;
};