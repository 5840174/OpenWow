#include "stdafx.h"

// General
#include "RenderPass_WMO.h"

// Additional
#include "WMO_Base_Instance.h"
#include "WMO_Group_Instance.h"

CRenderPass_WMO::CRenderPass_WMO(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline)
	: base(scene, pipeline)
{
}

CRenderPass_WMO::~CRenderPass_WMO()
{}



//
// IVisitor
//
bool CRenderPass_WMO::Visit(std::shared_ptr<SceneNode3D> node)
{
    std::shared_ptr<CWMO_Base_Instance> wmoInstance = std::dynamic_pointer_cast<CWMO_Base_Instance>(node);
    std::shared_ptr<CWMO_Group_Instance> wmoGroupInstance = std::dynamic_pointer_cast<CWMO_Group_Instance>(node);

    if (!wmoInstance && !wmoGroupInstance)
        return false;

    return base::Visit(node);
}
