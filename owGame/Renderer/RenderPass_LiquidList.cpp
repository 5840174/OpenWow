#include "stdafx.h"

// General
#include "RenderPass_LiquidList.h"

// Additional
#include "Liquid/LiquidBaseInstance.h"

namespace
{
__declspec(align(16)) struct SLiquidColorsProperties
{
	SLiquidColorsProperties()
		: gRiverColorLight(1.0f, 1.0f, 1.0f)
		, gRiverColorDark(1.0f, 1.0f, 1.0f)
		, gRiverShallowAlpha(1.0f)
		, gRiverDeepAlpha(1.0f)

		, gOceanColorLight(1.0f, 1.0f, 1.0f)
		, gOceanColorDark(1.0f, 1.0f, 1.0f)
		, gOceanShallowAlpha(1.0f)
		, gOceanDeepAlpha(1.0f)
	{}

	ColorRGB gRiverColorLight;
	float gRiverShallowAlpha;
	ColorRGB gRiverColorDark;
	float gRiverDeepAlpha;

	ColorRGB gOceanColorLight;
	float gOceanShallowAlpha;
	ColorRGB gOceanColorDark;
	float gOceanDeepAlpha;

	//-------------------------- ( 64 bytes )
};
}


CRenderPass_LiquidList::CRenderPass_LiquidList(IRenderDevice& RenderDevice, const std::shared_ptr<IRenderPassCreateTypelessList>& CreateTypelessList)
	: CRenderPassPipelinedProcessTypelessList(RenderDevice, CreateTypelessList)
	, m_SkyManager(nullptr)
{
	SetPassName("LiquidList");

	m_LiquidColorsConstantBuffer = GetRenderDevice().GetObjectsFactory().CreateConstantBuffer(nullptr, sizeof(SLiquidColorsProperties));
}

CRenderPass_LiquidList::~CRenderPass_LiquidList()
{}




//
// IRenderPass
//
void CRenderPass_LiquidList::Render(RenderEventArgs & e)
{
	SLiquidColorsProperties liquidColors;
	if (m_SkyManager != nullptr)
	{
		liquidColors.gRiverColorLight   = m_SkyManager->GetColor(ESkyColors::SKY_COLOR_RIVER_LIGHT);
		liquidColors.gRiverColorDark    = m_SkyManager->GetColor(ESkyColors::SKY_COLOR_RIVER_DARK);
		liquidColors.gRiverShallowAlpha = m_SkyManager->GetWaterAlpha(ESkyWaterAlpha::SKY_WATER_SHALLOW);
		liquidColors.gRiverDeepAlpha    = m_SkyManager->GetWaterAlpha(ESkyWaterAlpha::SKY_WATER_DEEP);

		liquidColors.gOceanColorLight   = m_SkyManager->GetColor(ESkyColors::SKY_COLOR_OCEAN_LIGHT);
		liquidColors.gOceanColorDark    = m_SkyManager->GetColor(ESkyColors::SKY_COLOR_OCEAN_DARK);
		liquidColors.gOceanShallowAlpha = m_SkyManager->GetWaterAlpha(ESkyWaterAlpha::SKY_OCEAN_SHALLOW);
		liquidColors.gOceanDeepAlpha    = m_SkyManager->GetWaterAlpha(ESkyWaterAlpha::SKY_OCEAN_DEEP);
	}
	else
	{
		if (GetBaseManager().IsManagerExists<CSkyManager>())
			m_SkyManager = GetBaseManager().GetManager<CSkyManager>();
	}

	if (m_LiquidColorsConstantBuffer != nullptr)
	{
		m_LiquidColorsConstantBuffer->Set(liquidColors);
	}

	if (m_ShaderLiquidColorsParameter && m_LiquidColorsConstantBuffer)
	{
		m_ShaderLiquidColorsParameter->SetConstantBuffer(m_LiquidColorsConstantBuffer);
		m_ShaderLiquidColorsParameter->Bind();
	}

	__super::Render(e);

	if (m_ShaderLiquidColorsParameter)
	{
		m_ShaderLiquidColorsParameter->Unbind();
	}
}



//
// IRenderPassPipelined
//
std::shared_ptr<IRenderPassPipelined> CRenderPass_LiquidList::ConfigurePipeline(std::shared_ptr<IRenderTarget> RenderTarget)
{
	__super::ConfigurePipeline(RenderTarget);

	auto vertexShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::VertexShader, "shaders_D3D/Liquid.hlsl", "VS_main");
	vertexShader->LoadInputLayoutFromReflector();

	auto pixelShader = GetRenderDevice().GetObjectsFactory().LoadShader(EShaderType::PixelShader, "shaders_D3D/Liquid.hlsl", "PS_main");

	GetPipeline().GetBlendState()->SetBlendMode(alphaBlending);
	GetPipeline().GetDepthStencilState()->SetDepthMode(enableDepthWrites);
	GetPipeline().GetRasterizerState()->SetCullMode(IRasterizerState::CullMode::None);
	GetPipeline().GetRasterizerState()->SetFillMode(IRasterizerState::FillMode::Solid, IRasterizerState::FillMode::Solid);
	GetPipeline().SetShader(vertexShader);
	GetPipeline().SetShader(pixelShader);

	m_ShaderLiquidColorsParameter = pixelShader->GetShaderParameterByName("LiquidColors");
	_ASSERT(m_ShaderLiquidColorsParameter != nullptr);

	return shared_from_this();
}



//
// IVisitor
//
EVisitResult CRenderPass_LiquidList::Visit(const std::shared_ptr<ISceneNode>& SceneNode3D)
{
	if (auto liquidInstance = std::dynamic_pointer_cast<CLiquidBaseInstance>(SceneNode3D))
		return __super::Visit(liquidInstance);

    return EVisitResult::Block;
}
