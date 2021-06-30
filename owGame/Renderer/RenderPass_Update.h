#pragma once

#include "RenderPassProcessTypelessList.h"

class ZN_API CRenderPass_Update
	: public CRenderPassProcessTypelessList
{
public:
	CRenderPass_Update(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList);
	virtual ~CRenderPass_Update();

    // IVisitor
    EVisitResult Visit(const std::shared_ptr<ISceneNode>& node) override final;
};
