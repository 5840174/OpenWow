#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "RenderPass_WMOList.h"

// Additional
#include "WMO/WMO_Base_Instance.h"
#include "WMO/WMO_Group_Instance.h"
#include "WMO/WMO_Part_Material.h"

CRenderPass_WMOList::CRenderPass_WMOList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: CRenderPassPipelinedProcessTypelessList(RenderDevice, CreateTypelessList)
	, m_CurrentWMOGroupInstance(nullptr)
	, m_CurrentWMOBatch(nullptr)
{
	SetPassName("WMOList");

	m_WoWSettings = GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings");
}

CRenderPass_WMOList::~CRenderPass_WMOList()
{}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_WMOList::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	std::shared_ptr<IShader> g_pVertexShader;
	std::shared_ptr<IShader> g_pPixelShader;

	if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_DirectX11)
	{
		g_pVertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/WMO.hlsl", "VS_main");
		g_pPixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/WMO.hlsl", "PS_main");
	}
	else if (GetRenderDevice().GetDeviceType() == RenderDeviceType::RenderDeviceType_OpenGL)
	{
		g_pVertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_OGL/WMO.vs", "");
		g_pPixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_OGL/WMO.ps", "");
	}
	g_pVertexShader->LoadInputLayoutFromReflector();

	// PIPELINES
	GetPipeline().GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	GetPipeline().SetRenderTarget(RenderTarget);
	//GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	//GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Wireframe, IRasterizerState::FillMode::Wireframe);
	GetPipeline().SetShader(g_pVertexShader);
	GetPipeline().SetShader(g_pPixelShader);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_WMOList::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	//if (std::dynamic_pointer_cast<CWMO_Base_Instance>(SceneNode))
	//{
	//	return __super::Visit(SceneNode);
	//}
	m_CurrentWMOGroupInstance = nullptr;
	if (auto wmoGroupInstance = std::dynamic_pointer_cast<CWMO_Group_Instance>(SceneNode))
	{
		m_CurrentWMOGroupInstance = wmoGroupInstance.get();
		return __super::Visit(SceneNode);
	}

	return EVisitResult::Block;
}

EVisitResult CRenderPass_WMOList::Visit(const std::shared_ptr<IModel>& Model)
{
	m_CurrentWMOBatch = nullptr;
	if (auto wmoBatch = std::dynamic_pointer_cast<WMO_Group_Part_Batch>(Model))
	{
		m_CurrentWMOBatch = wmoBatch.get();
		return EVisitResult::AllowAll;
	}

	return EVisitResult::Block;
}

EVisitResult CRenderPass_WMOList::Visit(const std::shared_ptr<IGeometry>& Geometry, const std::shared_ptr<IMaterial>& Material, SGeometryDrawArgs GeometryDrawArgs)
{
	auto wmoMaterial = std::dynamic_pointer_cast<WMO_Part_Material>(Material);

	//if (m_CurrentWMOBatch)
	//	wmoMaterial->SetMOCVExists(m_CurrentWMOGroupInstance->GetWMOGroup().GetHeader().flags.HAS_MOCV && m_CurrentWMOBatch->GetBatchType() != WMO_Group_Part_Batch::EBatchType::BatchType_Ext);

	wmoMaterial->GetBlendState()->Bind();
	wmoMaterial->GetRasterizerState()->Bind();

	EVisitResult visitResult = __super::Visit(Geometry, Material, GeometryDrawArgs);

	wmoMaterial->GetBlendState()->Unbind();
	wmoMaterial->GetRasterizerState()->Unbind();

	return visitResult;
}

#endif