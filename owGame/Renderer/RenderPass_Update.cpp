#include "stdafx.h"

// General
#include "RenderPass_Update.h"

CRenderPass_Update::CRenderPass_Update(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: CRenderPassProcessTypelessList(RenderDevice, CreateTypelessList)
{
	SetPassName("UpdatePass");
}

CRenderPass_Update::~CRenderPass_Update()
{}



//
// IVisitor
//
EVisitResult CRenderPass_Update::Visit(const std::shared_ptr<ISceneNode>& SceneNode3D)
{
	if (false == SceneNode3D->IsUpdateEnabled())
	{
		for (const auto& componentsIt : SceneNode3D->GetComponents())
			if (componentsIt.second)
				componentsIt.second->Update(GetRenderEventArgs());
		const_cast<ISceneNode*>(SceneNode3D.get())->Update(GetRenderEventArgs());
	}

    return EVisitResult::Block;
}
