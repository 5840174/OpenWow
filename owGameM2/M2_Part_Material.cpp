#include "stdafx.h"

// General
#include "M2_Part_Material.h"

// M2Blend converter
struct
{
	SM2_Material::BlendModes	M2Blend;
	uint8						EGxBLend;
} M2Blend_To_EGxBlend[SM2_Material::COUNT] =
{
	{ SM2_Material::M2BLEND_OPAQUE,			0 },
	{ SM2_Material::M2BLEND_ALPHA_KEY,		1 },
	{ SM2_Material::M2BLEND_ALPHA,			2 },
	{ SM2_Material::M2BLEND_NO_ALPHA_ADD,	10 },
	{ SM2_Material::M2BLEND_ADD,			3 },
	{ SM2_Material::M2BLEND_MOD,			4 },
	{ SM2_Material::M2BLEND_MOD2X,			5 }
};

CM2_Part_Material::CM2_Part_Material(const SM2_Material& _proto)
{
	m_IsLightingDisable = _proto.flags.UNLIT;

	m_IsFogDisable = _proto.flags.UNFOGGED;
	m_CullMode = (_proto.flags.TWOSIDED != 0) ? IRasterizerState::CullMode::None : IRasterizerState::CullMode::Back;
	m_DepthTest = (_proto.flags.DEPTHTEST == 0);
	m_DepthWrite = (_proto.flags.DEPTHWRITE == 0) ? IDepthStencilState::DepthWrite::Enable : IDepthStencilState::DepthWrite::Disable;

	m_M2BlendMode = _proto.m_BlendMode;
}

void CM2_Part_Material::Set() const
{
	/*_Render->r.setCullMode(m_IsTwoSided ? R_CullMode::RS_CULL_NONE : R_CullMode::RS_CULL_BACK);
	_Render->r.setDepthTest(m_DepthTestEnabled);
	_Render->r.setDepthMask(m_DepthMaskEnabled);
	_Render->getRenderStorage()->SetEGxBlend(_Render->r.getState(), M2Blend_To_EGxBlend[m_M2BlendMode].EGxBLend);*/
}

IDepthStencilState::DepthMode CM2_Part_Material::GetDepthMode() const
{
	return IDepthStencilState::DepthMode(m_DepthTest, m_DepthWrite);
}

/*IBlendState::BlendMode CM2_Part_Material::GetBlendMode() const
{
	switch (m_M2BlendMode)
	{
	case 0: // Opaque
		return IBlendState::BlendMode(false, false,
			IBlendState::BlendFactor::One,
			IBlendState::BlendFactor::Zero);
		break;

	case 1: // Combiners_Mod
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
		break;

	case 2: // Combiners_Decal 
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha);
		break;

	case 3: // Combiners_Add 
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcColor, IBlendState::BlendFactor::DstColor,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::DstAlpha);
		break;

	case 4: // Combiners_Mod2x 
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One);
		break;

	case 5: // Combiners_Fade 
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha);
		break;


	default:
		_ASSERT(false);
	}
}*/

IBlendState::BlendMode CM2_Part_Material::GetBlendMode() const
{
	switch (M2Blend_To_EGxBlend[m_M2BlendMode].EGxBLend)
	{
	case 0: // Opaque
		return IBlendState::BlendMode(false, false,
			IBlendState::BlendFactor::One,
			IBlendState::BlendFactor::Zero);
		break;

	case 1: // AlphaKey
		return IBlendState::BlendMode(false, false,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
		break;

	case 2: // Alpha
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha);
		break;

	case 3: // Add
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::Zero, IBlendState::BlendFactor::One);
		break;

	case 4: // Mod
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::Zero,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::Zero);
		break;

	case 5: // Mod2x
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::SrcColor,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::SrcAlpha);
		break;

	case 6: // ModAdd
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::One);
		break;

	case 7: // InvSrcAlphaAdd
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::One);
		break;

	case 8: // InvSrcAlphaOpaque
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::Zero,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::Zero);
		break;

	case 9: // SrcAlphaOpaque
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::Zero,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::Zero);
		break;

	case 10: // NoAlphaAdd
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::Zero, IBlendState::BlendFactor::One);

	case 11: // ConstantAlpha
		//(true, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
		fail2("Constant alpha EGxBlend doesn't support");
		break;

	case 12: // Screen
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::OneMinusDstColor, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
		break;

	case 13: // BlendAdd
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha);
		break;

	default:
		_ASSERT(false);
	}
}

IRasterizerState::CullMode CM2_Part_Material::GetCullMode() const
{
	return m_CullMode;
}
