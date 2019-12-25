#include "stdafx.h"

// General
#include "RenderPass_WDL.h"

// Additional
#include "WDL_LowResTile.h"
#include "Map.h"

CRenderPass_WDL::CRenderPass_WDL(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline)
	: Base3DPass(RenderDevice, scene, pipeline)
{
	m_WoWSettings = RenderDevice->GetBaseManager()->GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WDL::~CRenderPass_WDL()
{}



//
// IVisitor
//
bool CRenderPass_WDL::Visit3D(ISceneNode* node)
{
    CMap* map = dynamic_cast<CMap*>(node);
    if (map == nullptr)
        return false;

    return Base3DPass::Visit3D(node);
}

bool CRenderPass_WDL::Visit(IMesh* Mesh, UINT IndexStartLocation, UINT IndexCnt, UINT VertexStartLocation, UINT VertexCnt)
{
    CWDL_LowResTile* wdlMesh = dynamic_cast<CWDL_LowResTile*>(Mesh);
	if (wdlMesh == nullptr)
        return false;

	return Mesh->Render(GetRenderEventArgs(), GetPerObjectConstantBuffer().get(), IndexStartLocation, IndexCnt, VertexStartLocation, VertexCnt);
}
