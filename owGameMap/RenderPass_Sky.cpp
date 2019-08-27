#include "stdafx.h"

// General
#include "RenderPass_Sky.h"

// Additional 
#include "SkyManager.h"

CRenderPass_Sky::CRenderPass_Sky(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline)
	: base(scene, pipeline)
{
}

CRenderPass_Sky::~CRenderPass_Sky()
{}



//
// IVisitor
//
bool CRenderPass_Sky::Visit(SceneNode3D* node)
{
    SkyManager* skyManagerInstance = dynamic_cast<SkyManager*>(node);
    if (skyManagerInstance == nullptr)
        return false;

    return base::Visit(node);
}
