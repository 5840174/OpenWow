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

CRenderPass_M2::CRenderPass_M2(std::shared_ptr<IRenderDevice> RenderDevice, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
{}

CRenderPass_M2::~CRenderPass_M2()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_M2::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	// CreateShaders
	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;
	if (GetRenderDevice()->GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = GetRenderDevice()->CreateShader(
			EShaderType::VertexShader, "shaders_D3D/M2.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
		);
		g_pVertexShader->LoadInputLayoutFromReflector();

		g_pPixelShader = GetRenderDevice()->CreateShader(
			EShaderType::PixelShader, "shaders_D3D/M2.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
		);
	}
	else if (GetRenderDevice()->GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		_ASSERT(false);
	}

	// PIPELINES
	std::shared_ptr<IPipelineState> M2Pipeline = GetRenderDevice()->CreatePipelineState();
	M2Pipeline->GetBlendState()->SetBlendMode(alphaBlending);
	M2Pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	M2Pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	M2Pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	M2Pipeline->SetRenderTarget(RenderTarget);
	M2Pipeline->GetRasterizerState()->SetViewport(Viewport);
	M2Pipeline->SetShader(EShaderType::VertexShader, g_pVertexShader);
	M2Pipeline->SetShader(EShaderType::PixelShader, g_pPixelShader);

	return SetPipeline(M2Pipeline);
}



//
// IVisitor
//
bool CRenderPass_M2::Visit3D(ISceneNode* node)
{
    CM2_Base_Instance* m2Instance = dynamic_cast<CM2_Base_Instance*>(node);
    if (m2Instance)
    {
        return Base3DPass::Visit3D(node);
    }

    return false;
}

bool CRenderPass_M2::Visit(IMesh* Mesh, SGeometryPartParams GeometryPartParams)
{
	if (CM2_Skin_Batch* pMesh = dynamic_cast<CM2_Skin_Batch*>(Mesh))
	{
		/*GetPipelineState()->GetBlendState().SetBlendMode(pMesh->GetM2Material()->GetBlendMode());
		GetPipelineState()->GetBlendState().Bind();

		GetPipelineState()->GetDepthStencilState().SetDepthMode(pMesh->GetM2Material()->GetDepthMode());
		GetPipelineState()->GetDepthStencilState().Bind();

		GetPipelineState()->GetRasterizerState().SetCullMode(pMesh->GetM2Material()->GetCullMode());
		GetPipelineState()->GetRasterizerState().Bind();

		GetPipelineState()->Bind();*/

        return Mesh->Render(GetRenderEventArgs(), m_PerObjectConstantBuffer.get(), GeometryPartParams);
	}

    return false;
}
