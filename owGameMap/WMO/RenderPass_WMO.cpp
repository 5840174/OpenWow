#include "stdafx.h"

// General
#include "RenderPass_WMO.h"

// Additional
#include "WMO_Base_Instance.h"
#include "WMO_Group_Instance.h"

CRenderPass_WMO::CRenderPass_WMO(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline)
	: Base3DPass(RenderDevice, scene, pipeline)
{
	m_WoWSettings = RenderDevice->GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WMO::~CRenderPass_WMO()
{}



//
// IVisitor
//
bool CRenderPass_WMO::Visit3D(ISceneNode* node)
{
    CWMO_Base_Instance* wmoInstance = dynamic_cast<CWMO_Base_Instance*>(node);
	CWMO_Group_Instance* wmoGroupInstance = dynamic_cast<CWMO_Group_Instance*>(node);

    if (!wmoInstance && !wmoGroupInstance)
        return false;

    return Base3DPass::Visit3D(node);
}
