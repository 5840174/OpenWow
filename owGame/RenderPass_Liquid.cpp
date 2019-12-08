#include "stdafx.h"

// General
#include "RenderPass_Liquid.h"

// Additional
#include "LiquidInstance.h"

CRenderPass_Liquid::CRenderPass_Liquid(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline)
	: Base3DPass(RenderDevice, scene, pipeline)
{}

CRenderPass_Liquid::~CRenderPass_Liquid()
{}



//
// IVisitor
//
bool CRenderPass_Liquid::Visit(ISceneNode3D* node)
{
    Liquid_Instance* liquidInstance = dynamic_cast<Liquid_Instance*>(node);
    if (liquidInstance)
    {
        return Base3DPass::Visit(node);
    }

    return false;
}
