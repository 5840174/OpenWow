#include "stdafx.h"

// General
#include "RenderPass_M2.h"

// Additional
#include "M2_Part_Color.h"
#include "M2_Part_Material.h"
#include "M2_Part_Texture.h"
#include "M2_Part_TextureTransform.h"
#include "M2_Part_TextureWeight.h"

// Additional (SceneNodes)
#include "M2_Base_Instance.h"

// Additional (meshes)
#include "M2_Skin_Batch.h"

CRenderPass_M2::CRenderPass_M2(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene, std::shared_ptr<IPipelineState> pipeline)
	: Base3DPass(RenderDevice, scene, pipeline)
{}

CRenderPass_M2::~CRenderPass_M2()
{}



//
// IVisitor
//
bool CRenderPass_M2::Visit(ISceneNode3D* node)
{
    CM2_Base_Instance* m2Instance = dynamic_cast<CM2_Base_Instance*>(node);
    if (m2Instance)
    {
        return Base3DPass::Visit(node);
    }

    return false;
}

bool CRenderPass_M2::Visit(IMesh* Mesh, UINT IndexStartLocation, UINT IndexCnt, UINT VertexStartLocation, UINT VertexCnt)
{
	CM2_Skin_Batch* pMesh = dynamic_cast<CM2_Skin_Batch*>(Mesh);
	if (pMesh)
	{
		/*GetPipelineState()->GetBlendState().SetBlendMode(pMesh->GetM2Material()->GetBlendMode());
		GetPipelineState()->GetBlendState().Bind();

		GetPipelineState()->GetDepthStencilState().SetDepthMode(pMesh->GetM2Material()->GetDepthMode());
		GetPipelineState()->GetDepthStencilState().Bind();

		GetPipelineState()->GetRasterizerState().SetCullMode(pMesh->GetM2Material()->GetCullMode());
		GetPipelineState()->GetRasterizerState().Bind();

		GetPipelineState()->Bind();*/

        return Mesh->Render(GetRenderEventArgs(), GetPerObjectConstantBuffer().get(), IndexStartLocation, IndexCnt, VertexStartLocation, VertexCnt);
	}

    return false;
}
