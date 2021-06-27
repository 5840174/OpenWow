#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include
#include "Wmo.h"

// General
#include "Wmo_Part_Material.h"

WMO_Part_Material::WMO_Part_Material(IRenderDevice& RenderDevice, const CWMO& WMOModel, const SWMO_MaterialDef& WMOMaterialProto)
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("WMO_Part_Material"))
	, m_WMOModel(WMOModel)
{
	MaterialData().BlendMode = WMOMaterialProto.blendMode;

	// Create samplers
	std::shared_ptr<ISamplerState> sampler = RenderDevice.GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	sampler->SetWrapMode(
		WMOMaterialProto.flags.TextureClampS ? ISamplerState::WrapMode::Clamp : ISamplerState::WrapMode::Repeat,
		WMOMaterialProto.flags.TextureClampT ? ISamplerState::WrapMode::Clamp : ISamplerState::WrapMode::Repeat
	);
    SetSampler(0, sampler);

	// This
	std::string textureName = m_WMOModel.GetTextureName(WMOMaterialProto.diffuseNameIndex);
	std::shared_ptr<ITexture> texture = GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(textureName);
	SetTexture(0, texture);

	//if (m_WMOMaterialProto.envNameIndex)
	//{
	//	SetTexture(1, RenderDevice->CreateTexture2D(_parentWMO.lock()->m_TexturesNames + m_WMOMaterialProto.envNameIndex));
	//}

	//Log::Warn("Shader = [%d], Blend mode [%d]", m_WMOMaterialProto.shader, m_WMOMaterialProto.blendMode);

	glm::vec4 color = fromARGB(WMOMaterialProto.diffColor);

	m_BlendState = RenderDevice.GetBaseManager().GetManager<IWoWObjectsCreator>()->GetEGxBlend(WMOMaterialProto.blendMode);

	m_RasterizerState = RenderDevice.GetObjectsFactory().CreateRasterizerState();
	m_RasterizerState->SetCullMode((WMOMaterialProto.flags.IsTwoSided != 0) ? IRasterizerState::CullMode::None : IRasterizerState::CullMode::Back);
}

WMO_Part_Material::~WMO_Part_Material()
{
}

void WMO_Part_Material::SetBlendMode(uint32 BlendMode)
{
	MaterialData().BlendMode = BlendMode;
}

void WMO_Part_Material::SetMOCVExists(uint32 MOCVExists)
{
	MaterialData().MOCVExists = MOCVExists;
}

/*void WMO_Part_Material::fillRenderState(RenderState* _state) const
{
	uint16 sampler = m_QualitySettings.Texture_Sampler;
	sampler |= (m_WMOMaterialProto.flags.TextureClampS) ? SS_ADDRU_CLAMP : SS_ADDRU_WRAP;
	sampler |= (m_WMOMaterialProto.flags.TextureClampT) ? SS_ADDRV_CLAMP : SS_ADDRV_WRAP;

	_state->setTexture(Material::C_DiffuseTextureIndex + 0, m_DiffuseTexture[0], sampler, 0);
	_state->setTexture(Material::C_DiffuseTextureIndex + 1, m_DiffuseTexture[1], sampler, 0);
	_state->setCullMode(m_WMOMaterialProto.flags.IsTwoSided ? RS_CULL_NONE : RS_CULL_BACK);
	_Render->getRenderStorage()->SetEGxBlend(_state, m_WMOMaterialProto.blendMode);
}*/

/*void WMO_Part_Material::set() const
{
	uint16 sampler = m_QualitySettings.Texture_Sampler;
	sampler |= (m_WMOMaterialProto.flags.TextureClampS) ? SS_ADDRU_CLAMP : SS_ADDRU_WRAP;
	sampler |= (m_WMOMaterialProto.flags.TextureClampT) ? SS_ADDRV_CLAMP : SS_ADDRV_WRAP;

	_Render->r.setTexture(Material::C_DiffuseTextureIndex + 0, m_DiffuseTexture[0], sampler, 0);
	_Render->r.setTexture(Material::C_DiffuseTextureIndex + 1, m_DiffuseTexture[1], sampler, 0);

	_Render->r.setCullMode(m_WMOMaterialProto.flags.IsTwoSided ? RS_CULL_NONE : RS_CULL_BACK);

	//_Render->r.setAlphaToCoverage(true);

	_Render->getRenderStorage()->SetEGxBlend(_Render->r.getState(), m_WMOMaterialProto.blendMode);
}*/

#endif
