#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "RenderPass_M2CollisionDebug.h"

// Additional
#include "M2/M2_Base_Instance.h"
#include "WMO/WMO_Base_Instance.h"

CRenderPass_M2CollisionDebug::CRenderPass_M2CollisionDebug(IScene& Scene)
	: Base3DPass(Scene)
{
	SetPassName("M2CollisionDebug");
}

CRenderPass_M2CollisionDebug::~CRenderPass_M2CollisionDebug()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_M2CollisionDebug::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	m_MaterialDebug = MakeShared(MaterialDebug, GetRenderDevice());

	auto vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "3D/Debug.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	auto pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "3D/Debug.hlsl", "PS_main");
	
	// PIPELINES
	//GetPipeline().GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	//GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Wireframe, IRasterizerState::FillMode::Wireframe);
	GetPipeline().SetRenderTarget(RenderTarget);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_M2CollisionDebug::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	/*if (auto m2Instance = std::dynamic_pointer_cast<CM2_Base_Instance>(SceneNode))
	{
		if (auto collisionGeom = m2Instance->GetM2().GetCollisionGeometry())
		{
			BindPerObjectData(PerObject(SceneNode->GetWorldTransfom()));

			m_MaterialDebug->SetDiffuseColor(ColorRGBA(0.0f, 1.0f, 1.0f, 1.0f));
			m_MaterialDebug->Bind(GetPipeline().GetPixelShaderPtr());
			collisionGeom->Render(GetPipeline().GetVertexShaderPtr());
			m_MaterialDebug->Unbind(GetPipeline().GetPixelShaderPtr());
		}
	}
	else */
	/*if (auto wmoGroupInstance = std::dynamic_pointer_cast<CWMO_Group_Instance>(SceneNode))
	{
		BindPerObjectData(PerObject(SceneNode->GetWorldTransfom()));

		if (auto collisionGeom = wmoGroupInstance->GetWMOGroup().GetCollisionGeometry())
		{
			m_MaterialDebug->SetDiffuseColor(ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f));
			m_MaterialDebug->Bind(GetPipeline().GetPixelShaderPtr());
			collisionGeom->Render(GetPipeline().GetVertexShaderPtr());
			m_MaterialDebug->Unbind(GetPipeline().GetPixelShaderPtr());
		}
	}*/
	if (auto wmoGroupInstance = std::dynamic_pointer_cast<CWMO_Group_Instance>(SceneNode))
	{
		BindPerObjectData(PerObject(SceneNode->GetWorldTransfom()));

		for (const auto& collisionNode : wmoGroupInstance->GetWMOGroup().GetCollisionNodes())
		{
			if (auto collisionGeom = collisionNode->GetCollisionGeometry())
			{

				m_MaterialDebug->SetDiffuseColor(ColorRGBA(1.0f, 0.0f, 1.0f, 1.0f));
				m_MaterialDebug->Bind(GetPipeline().GetPixelShaderPtr());
				collisionGeom->Render(GetPipeline().GetVertexShaderPtr());
				m_MaterialDebug->Unbind(GetPipeline().GetPixelShaderPtr());
			}
		}
	}

	return EVisitResult::AllowVisitChilds;
}

EVisitResult CRenderPass_M2CollisionDebug::Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs)
{
	return EVisitResult::Block;
}

#endif