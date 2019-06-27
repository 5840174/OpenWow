#include "stdafx.h"

// General
#include "RenderPass_ADT_MCNK.h"

// Additional
#include "MapChunk.h"

CRenderPass_ADT_MCNK::CRenderPass_ADT_MCNK(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline)
	: BasePass(scene, pipeline)
	, m_QualitySettings(GetSettingsGroup<CGroupQuality>())
{
}

CRenderPass_ADT_MCNK::~CRenderPass_ADT_MCNK()
{}



//
// IVisitor
//
bool CRenderPass_ADT_MCNK::Visit(std::shared_ptr<SceneNode3D> node)
{
    std::shared_ptr<CMapChunk> adtMCNKInstance = std::dynamic_pointer_cast<CMapChunk, SceneNode3D>(node);
    if (adtMCNKInstance == nullptr)
        return false;

    return base::Visit(node);
}
