#include "stdafx.h"

// General
#include "RenderPass_WMO.h"

// Additional
#include "WMO_Base_Instance.h"
#include "WMO_Group_Instance.h"
#include "WMO/WMO_Part_Material.h"

CRenderPass_WMO::CRenderPass_WMO(IRenderDevice& RenderDevice, const std::shared_ptr<CSceneCreateTypedListsPass>& SceneNodeListPass)
	: CBaseList3DPass(RenderDevice, SceneNodeListPass, cWMOGroup_NodeType)
{
	m_WoWSettings = RenderDevice.GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WMO::~CRenderPass_WMO()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_WMO::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;

	if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_D3D/WMO.hlsl", "VS_main");
		g_pPixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "shaders_D3D/WMO.hlsl", "PS_main");
	}
	else if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		g_pVertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_OGL/WMO.vs", "");
		g_pPixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "shaders_OGL/WMO.ps", "");
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->SetShader(EShaderType::VertexShader, g_pVertexShader);
	pipeline->SetShader(EShaderType::PixelShader, g_pPixelShader);

	return SetPipeline(pipeline);
}



//
// IVisitor
//
EVisitResult CRenderPass_WMO::Visit(const ISceneNode3D* SceneNode3D)
{
	if (SceneNode3D->Is(cWMO_NodeType) || SceneNode3D->Is(cWMOGroup_NodeType))
	{
		return CBaseList3DPass::Visit(SceneNode3D);
	}

	return EVisitResult::Block;
}

EVisitResult CRenderPass_WMO::Visit(const IGeometry * Geometry, const IMaterial * Material, SGeometryDrawArgs GeometryDrawArgs)
{
	auto wmoMaterial = static_cast<const WMO_Part_Material*>(Material);

	wmoMaterial->GetBlendState()->Bind();
	wmoMaterial->GetRasterizerState()->Bind();
	
	return CBaseList3DPass::Visit(Geometry, Material, GeometryDrawArgs);
}


#if 0

CRenderPass_WMO2::CRenderPass_WMO2(IRenderDevice & RenderDevice, const std::shared_ptr<BuildRenderListPassTemplated<CWMO_Group_Instance>>& List, std::shared_ptr<IScene> scene)
	: Base3DPass(RenderDevice, scene)
	, m_List(List)
{
	
}

CRenderPass_WMO2::~CRenderPass_WMO2()
{
}

struct Struct
{
	Struct(const ISceneNode3D* Node, const IGeometry* Geom, const SGeometryDrawArgs& GeometryDrawArgs)
		: Node(Node)
		, Geom(Geom)
		,GeometryDrawArgs(GeometryDrawArgs)
	{}

	const ISceneNode3D* Node;
	const IGeometry* Geom;
	SGeometryDrawArgs GeometryDrawArgs;
};

void CRenderPass_WMO2::Render(RenderEventArgs & e)
{
	std::unordered_map<const IMaterial*, std::vector<Struct>> m_Filled;

	//strBuffer = RenderDevice.GetObjectsFactory().CreateStructuredBuffer();

	for (const auto& it : m_List->GetGeometryList())
	{
		auto& result = m_Filled.find(it.Material);
		if (result == m_Filled.end())
		{
			m_Filled.insert
			(
				std::make_pair
				(
					it.Material, 
					std::vector<Struct>
					(
						{ Struct(it.Node, it.Geometry, it.GeometryDrawArgs) }
					)
				)
			);
		}
		else
		{
			result->second.push_back(Struct(it.Node, it.Geometry, it.GeometryDrawArgs));
		}
	}

	const IShader* vs = GetRenderEventArgs().PipelineState->GetShaders().at(EShaderType::VertexShader).get();
	for (const auto& it : m_Filled)
	{
		
		it.first->Bind(GetRenderEventArgs().PipelineState->GetShaders());
		for (const auto& it2 : it.second)
		{
			Visit(it.second.begin()->Node);
			it2.Geom->Render(GetRenderEventArgs(), vs, it2.GeometryDrawArgs);
		}
		it.first->Unbind(GetRenderEventArgs().PipelineState->GetShaders());
	}

	//Log::Info("Size = '%d'", m_Filled.size());
}

std::shared_ptr<IRenderPassPipelined> CRenderPass_WMO2::CreatePipeline(std::shared_ptr<IRenderTarget> RenderTarget, const Viewport * Viewport)
{
	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;

	if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX)
	{
		g_pVertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_D3D/WMO.hlsl", "VS_main");
		g_pPixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "shaders_D3D/WMO.hlsl", "PS_main");
	}
	else if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		g_pVertexShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::VertexShader, "shaders_OGL/WMO.vs", "");
		g_pPixelShader = GetRenderDevice().GetObjectsFactory().CreateShader(EShaderType::PixelShader, "shaders_OGL/WMO.ps", "");
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	std::shared_ptr<IPipelineState> pipeline = GetRenderDevice().GetObjectsFactory().CreatePipelineState();
	pipeline->GetBlendState()->SetBlendMode(disableBlending);
	pipeline->GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	pipeline->GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	pipeline->GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid);
	pipeline->SetRenderTarget(RenderTarget);
	pipeline->GetRasterizerState()->SetViewport(Viewport);
	pipeline->SetShader(EShaderType::VertexShader, g_pVertexShader);
	pipeline->SetShader(EShaderType::PixelShader, g_pPixelShader);

	return SetPipeline(pipeline);
}

#endif
