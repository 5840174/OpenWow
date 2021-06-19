#pragma once

#include "RenderPassProcessTypelessList.h"

class ZN_API CRenderPass_MapChunkList
	: public CRenderPassProcessTypelessList
{
public:
	CRenderPass_MapChunkList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPass_MapChunkList();

	// IRenderPassPipelined
	std::shared_ptr<IRenderPassPipelined> ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget) override final;

    // IVisitor
    EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;

private:
	std::shared_ptr<IPropertyT<float>> m_MapChunkRenderDistance;
};