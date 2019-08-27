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
bool CRenderPass_ADT_MCNK::Visit(SceneNode3D* node)
{
    CMapChunk* adtMCNKInstance = dynamic_cast<CMapChunk*>(node);
    if (adtMCNKInstance == nullptr)
        return false;

    return base::Visit(node);
}
