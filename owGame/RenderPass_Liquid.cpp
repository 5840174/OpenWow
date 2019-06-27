#include "stdafx.h"

// General
#include "RenderPass_Liquid.h"

// Additional
#include "LiquidInstance.h"

CRenderPass_Liquid::CRenderPass_Liquid(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline)
	: base(scene, pipeline)
{
}

CRenderPass_Liquid::~CRenderPass_Liquid()
{}



//
// IVisitor
//
bool CRenderPass_Liquid::Visit(std::shared_ptr<SceneNode3D> node)
{
    std::shared_ptr<Liquid_Instance> liquidInstance = std::dynamic_pointer_cast<Liquid_Instance, SceneNode3D>(node);
    if (liquidInstance)
    {
        return base::Visit(node);
    }

    return false;
}
