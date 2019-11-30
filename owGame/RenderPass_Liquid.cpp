#include "stdafx.h"

// General
#include "RenderPass_Liquid.h"

// Additional
#include "LiquidInstance.h"

CRenderPass_Liquid::CRenderPass_Liquid(std::shared_ptr<Scene3D> scene, std::shared_ptr<IPipelineState> pipeline)
	: base(scene, pipeline)
{
}

CRenderPass_Liquid::~CRenderPass_Liquid()
{}



//
// IVisitor
//
bool CRenderPass_Liquid::Visit(SceneNode3D* node)
{
    Liquid_Instance* liquidInstance = dynamic_cast<Liquid_Instance*>(node);
    if (liquidInstance)
    {
        return base::Visit(node);
    }

    return false;
}
