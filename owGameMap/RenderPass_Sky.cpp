#include "stdafx.h"

// General
#include "RenderPass_Sky.h"

// Additional 
#include "SkyManager.h"

CRenderPass_Sky::CRenderPass_Sky(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline)
	: Base3DPass(RenderDevice, scene, pipeline)
{
	m_WoWSettings = GetManager<ISettings>(RenderDevice->GetBaseManager())->GetGroup("WoWSettings");
}

CRenderPass_Sky::~CRenderPass_Sky()
{}



//
// IVisitor
//
bool CRenderPass_Sky::Visit(ISceneNode3D* node)
{
    SkyManager* skyManagerInstance = dynamic_cast<SkyManager*>(node);
    if (skyManagerInstance == nullptr)
        return false;

    return Base3DPass::Visit(node);
}
