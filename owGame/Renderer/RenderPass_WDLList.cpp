#include "stdafx.h"

// General
#include "RenderPass_WDLList.h"

// Additional
#include "Map/MapWDLTileModel.h"
#include "Map/Map.h"

__declspec(align(16)) struct SColorProperties
{
	SColorProperties()
		: DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f)
	{}

	ColorRGBA DiffuseColor;
	//-------------------------- ( 16 bytes )
};

CRenderPass_WDLList::CRenderPass_WDLList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: CRenderPassPipelinedProcessTypelessList(RenderDevice, CreateTypelessList)
	, m_SkyManager(nullptr)
{
	SetPassName("MapWDLList");

	m_ColorConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(nullptr, sizeof(SColorProperties));
}

CRenderPass_WDLList::~CRenderPass_WDLList()
{}



//
// IRenderPass
//
void CRenderPass_WDLList::Render(RenderEventArgs & e)
{
	SColorProperties color;
	if (m_SkyManager != nullptr)
	{
		color.DiffuseColor = ColorRGBA(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_RIVER_LIGHT), 1.0f);
	}
	else
	{
		if (GetBaseManager().IsManagerExists<CSkyManager>())
			m_SkyManager = GetBaseManager().GetManager<CSkyManager>();
	}


	if (m_ColorConstantBuffer != nullptr)
	{
		m_ColorConstantBuffer->Set(color);
	}

	if (m_ShaderColorParameter && m_ColorConstantBuffer)
	{
		m_ShaderColorParameter->SetConstantBuffer(m_ColorConstantBuffer);
		m_ShaderColorParameter->Bind();
	}

	__super::Render(e);

	if (m_ShaderColorParameter)
	{
		m_ShaderColorParameter->Unbind();
	}
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_WDLList::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	// CreateShaders
	auto vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/MapWDL.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	auto pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/MapWDL.hlsl", "PS_main");

	// PIPELINES
	GetPipeline().GetBlendState()->SetBlendMode(disableBlending);
	GetPipeline().GetDepthStencilState()->SetDepthMode(disableDepthWrites);
	GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::Back);
	GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	GetPipeline().SetRenderTarget(RenderTarget);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

	m_ShaderColorParameter = pixelShader->GetShaderParameterByName("Color");
	_ASSERT(m_ShaderColorParameter != nullptr);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_WDLList::Visit(const std::shared_ptr<ISceneNode>& SceneNode)
{
	if (const auto map = std::dynamic_pointer_cast<CMap>(SceneNode))
		return __super::Visit(SceneNode);

	return EVisitResult::Block;
}

EVisitResult CRenderPass_WDLList::Visit(const std::shared_ptr<IModel>& Model)
{
	const auto wdlMesh = std::dynamic_pointer_cast<CMapWDLTileModel>(Model);
	if (wdlMesh == nullptr)
        return EVisitResult::Block;

	if (false == wdlMesh->IsNeedRender())
		return EVisitResult::Block;

	return __super::Visit(wdlMesh);
}
