#include "stdafx.h"

// General
#include "RenderPass_WDL.h"

// Additional
#include "WDL_LowResTile.h"
#include "Map.h"

CRenderPass_WDL::CRenderPass_WDL(std::shared_ptr<Scene3D> scene, std::shared_ptr<IPipelineState> pipeline)
	: base(scene, pipeline)
{
}

CRenderPass_WDL::~CRenderPass_WDL()
{}



//
// IVisitor
//
bool CRenderPass_WDL::Visit(SceneNode3D* node)
{
    CMap* map = dynamic_cast<CMap*>(node);
    if (map == nullptr)
        return false;

    return base::Visit(node);
}

bool CRenderPass_WDL::Visit(IMesh* Mesh, UINT IndexStartLocation, UINT IndexCnt, UINT VertexStartLocation, UINT VertexCnt)
{
    CWDL_LowResTile* wdlMesh = dynamic_cast<CWDL_LowResTile*>(Mesh);
	if (wdlMesh == nullptr)
        return false;

	return Mesh->Render(GetRenderEventArgs(), GetPerObjectConstantBuffer().get(), IndexStartLocation, IndexCnt, VertexStartLocation, VertexCnt);
}
