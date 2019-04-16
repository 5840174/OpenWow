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

CRenderPass_M2::CRenderPass_M2(std::shared_ptr<Scene3D> scene, std::shared_ptr<PipelineState> pipeline)
	: BasePass(scene, pipeline)
{}

CRenderPass_M2::~CRenderPass_M2()
{}



//
// IVisitor
//
bool CRenderPass_M2::Visit(std::shared_ptr<SceneNode3D> node)
{
    std::shared_ptr<CM2_Base_Instance> m2Instance = std::dynamic_pointer_cast<CM2_Base_Instance, SceneNode3D>(node);
    if (m2Instance)
    {
        return base::Visit(node);
    }

    return false;
}

bool CRenderPass_M2::Visit(std::shared_ptr<IMesh> Mesh, UINT IndexStartLocation, UINT IndexCnt, UINT VertexStartLocation, UINT VertexCnt)
{
	std::shared_ptr<const CM2_Skin_Batch> pMesh = std::dynamic_pointer_cast<const CM2_Skin_Batch, IMesh>(Mesh);
	if (pMesh)
	{
		//GetPipelineState()->GetBlendState().SetBlendMode(pMesh->GetM2Material()->GetBlendMode());
		//GetPipelineState()->GetBlendState().Bind();

		//GetPipelineState()->GetDepthStencilState().SetDepthMode(pMesh->GetM2Material()->GetDepthMode());
		//GetPipelineState()->GetDepthStencilState().Bind();

		//GetPipelineState()->GetRasterizerState().SetCullMode(pMesh->GetM2Material()->GetCullMode());
		//GetPipelineState()->GetRasterizerState().Bind();

		//GetPipelineState()->Bind();
	}

	return Mesh->Render(GetRenderEventArgs(), GetPerObjectConstantBuffer(), IndexStartLocation, IndexCnt, VertexStartLocation, VertexCnt);
}
