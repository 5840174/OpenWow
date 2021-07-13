#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "RenderPass_WMOPortalDebug.h"

// Additional
#include "WMO/WMO_Base_Instance.h"
#include "WMO/WMO_Group_Instance.h"
#include "WMO/WMO_Part_Material.h"

CRenderPass_WMOPortalDebug::CRenderPass_WMOPortalDebug(IScene& Scene)
	: Base3DPass(Scene)
{
	SetPassName("WMOPortalDebug");

	m_WoWSettings = GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WMOPortalDebug::~CRenderPass_WMOPortalDebug()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_WMOPortalDebug::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	m_MaterialDebug = MakeShared(MaterialDebug, GetRenderDevice());

	std::shared_ptr<IShader> vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "3D/Debug.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "3D/Debug.hlsl", "PS_main");
	
	// PIPELINES
	GetPipeline().GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	GetPipeline().SetRenderTarget(RenderTarget);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_WMOPortalDebug::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	if (std::dynamic_pointer_cast<CWMO_Group_Instance>(SceneNode))
	{
		if (auto portalRoom = std::dynamic_pointer_cast<IPortalRoom>(SceneNode))
		{
			for (const auto& portal : portalRoom->GetPortals())
			{
#ifdef USE_WMO_PORTALS_CULLING
				if (auto portalCast = std::dynamic_pointer_cast<CWMOPortalInstance>(portal))
				{
					auto pointsXYZ = portalCast->GetVertices();
					pointsXYZ.push_back(pointsXYZ[0]);

					if (pointsXYZ.size() < 2)
						return EVisitResult::AllowVisitChilds;

					BindPerObjectData(PerObject());

					if (portalCast->IsPositive(GetRenderEventArgs().CameraForCulling->GetPosition()))
						m_MaterialDebug->SetDiffuseColor(ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f));
					else
						m_MaterialDebug->SetDiffuseColor(ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f));

					m_WaypointGeometry = GetRenderDevice().GetPrimitivesFactory().CreateLines(pointsXYZ);

					m_MaterialDebug->Bind(GetPipeline().GetPixelShaderPtr());
					m_WaypointGeometry->Render(GetPipeline().GetVertexShaderPtr());
					m_MaterialDebug->Unbind(GetPipeline().GetPixelShaderPtr());
				}
#endif
			}
		}
	}

	return EVisitResult::AllowVisitChilds;
}

EVisitResult CRenderPass_WMOPortalDebug::Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs)
{
	auto wmoMaterial = std::dynamic_pointer_cast<WMO_Part_Material>(Material);
	wmoMaterial->GetBlendState()->Bind();
	wmoMaterial->GetRasterizerState()->Bind();

	return EVisitResult::Block;
}

#endif