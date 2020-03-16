#include "stdafx.h"

// General
#include "M2_Part_Material.h"

// M2Blend converter
struct
{
	SM2_Material::BlendModes	M2Blend;
	uint8						EGxBLend;
} M2Blend_To_EGxBlend[SM2_Material::M2BLEND_MOD2X + 1] =
{
	{ SM2_Material::M2BLEND_OPAQUE,			0 },
	{ SM2_Material::M2BLEND_ALPHA_KEY,		1 },
	{ SM2_Material::M2BLEND_ALPHA,			2 },
	{ SM2_Material::M2BLEND_NO_ALPHA_ADD,	10 },
	{ SM2_Material::M2BLEND_ADD,			3 },
	{ SM2_Material::M2BLEND_MOD,			4 },
	{ SM2_Material::M2BLEND_MOD2X,			5 }
};

CM2_Part_Material::CM2_Part_Material(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2& M2Object, const SM2_Material& M2Material)
	: m_M2Object(M2Object)
{
	//m_IsLightingDisable = M2Material.flags.UNLIT;
	//m_IsFogDisable = M2Material.flags.UNFOGGED;

	m_M2BlendMode = /*M2Blend_To_EGxBlend[*/M2Material.BlendMode/*].EGxBLend*/;
	//Log::Green("Blend mode = %d (original %d)", M2Blend_To_EGxBlend[M2Material.BlendMode].EGxBLend, M2Material.BlendMode);

	m_BlendState = BaseManager.GetManager<IWoWObjectsCreator>()->GetEGxBlend(M2Blend_To_EGxBlend[M2Material.BlendMode].EGxBLend);

	m_DepthStencilState = RenderDevice.GetObjectsFactory().CreateDepthStencilState();
	m_DepthStencilState->SetDepthMode(IDepthStencilState::DepthMode(M2Material.flags.DEPTHTEST == 0, (M2Material.flags.DEPTHWRITE == 0) ? IDepthStencilState::DepthWrite::Enable : IDepthStencilState::DepthWrite::Disable));

	m_RasterizerState = RenderDevice.GetObjectsFactory().CreateRasterizerState();
	m_RasterizerState->SetCullMode((M2Material.flags.TWOSIDED != 0) ? IRasterizerState::CullMode::None : IRasterizerState::CullMode::Back);
}

CM2_Part_Material::~CM2_Part_Material()
{
}
