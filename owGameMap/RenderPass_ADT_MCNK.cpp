#include "stdafx.h"

// General
#include "RenderPass_ADT_MCNK.h"

// Additional
#include "MapChunk.h"

CRenderPass_ADT_MCNK::CRenderPass_ADT_MCNK(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline)
	: Base3DPass(RenderDevice, scene, pipeline)
{
	m_WoWSettings = RenderDevice->GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_ADT_MCNK::~CRenderPass_ADT_MCNK()
{}



//
// IVisitor
//
bool CRenderPass_ADT_MCNK::Visit3D(ISceneNode* node)
{
    CMapChunk* adtMCNKInstance = dynamic_cast<CMapChunk*>(node);
    if (adtMCNKInstance == nullptr)
        return false;

    return Base3DPass::Visit3D(node);
}
