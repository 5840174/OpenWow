#include "stdafx.h"

// Include
#include "Wmo.h"

// General
#include "Wmo_Part_Material.h"

WMO_Part_Material::WMO_Part_Material(const std::weak_ptr<const CWMO> _parentWMO, const SWMO_MaterialDef& _proto) :
	MaterialWrapper(_RenderDevice->CreateMaterial()),
	m_ParentWMO(_parentWMO),
	m_Proto(_proto),
	m_QualitySettings(GetSettingsGroup<CGroupQuality>())
{
	// Constant buffer
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	(*m_pProperties) = MaterialProperties();
	(*m_pProperties).m_BlendMode = m_Proto.blendMode;
	CreateConstantBuffer(m_pProperties, sizeof(MaterialProperties));

	// CreateShaders
    std::shared_ptr<Shader> g_pVertexShader;
    std::shared_ptr<Shader> g_pPixelShader;

    if (_RenderDevice->GetDeviceType() == IRenderDevice::DeviceType::DirectX)
    {
        g_pVertexShader = _RenderDevice->CreateShader(
            Shader::VertexShader, "shaders_D3D/WMO.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
        );

        g_pPixelShader = _RenderDevice->CreateShader(
            Shader::PixelShader, "shaders_D3D/WMO.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
        );
    }
    else if (_RenderDevice->GetDeviceType() == IRenderDevice::DeviceType::OpenGL)
    {
        g_pVertexShader = _RenderDevice->CreateShader(
            Shader::VertexShader, "shaders_OGL/WMO.vs", Shader::ShaderMacros(), "", ""
        );

        g_pPixelShader = _RenderDevice->CreateShader(
            Shader::PixelShader, "shaders_OGL/WMO.ps", Shader::ShaderMacros(), "", ""
        );
    }
    g_pVertexShader->LoadInputLayoutFromReflector();

	SetShader(Shader::VertexShader, g_pVertexShader);
	SetShader(Shader::PixelShader, g_pPixelShader);

	// Create samplers
	std::shared_ptr<SamplerState> g_Sampler = _RenderDevice->CreateSamplerState();
	g_Sampler->SetFilter(SamplerState::MinFilter::MinLinear, SamplerState::MagFilter::MagLinear, SamplerState::MipFilter::MipLinear);
	g_Sampler->SetWrapMode(
		m_Proto.flags.TextureClampS ? SamplerState::WrapMode::Clamp : SamplerState::WrapMode::Repeat, 
		m_Proto.flags.TextureClampT ? SamplerState::WrapMode::Clamp : SamplerState::WrapMode::Repeat
	);
    SetSampler(0, g_Sampler);

	// This
	std::string textureName = _parentWMO.lock()->m_TexturesNames + m_Proto.diffuseNameIndex;
	std::shared_ptr<Texture> texture = _RenderDevice->CreateTexture2D(textureName);
	SetTexture(0, texture);

	//if (m_Proto.envNameIndex)
	//{
	//	SetTexture(1, _RenderDevice->CreateTexture2D(_parentWMO.lock()->m_TexturesNames + m_Proto.envNameIndex));
	//}

	//Log::Warn("Shader = [%d], Blend mode [%d]", m_Proto.shader, m_Proto.blendMode);

	vec4 color = fromARGB(m_Proto.diffColor);
}

WMO_Part_Material::~WMO_Part_Material()
{
	if (m_pProperties)
	{
		_aligned_free(m_pProperties);
		m_pProperties = nullptr;
	}
}

void WMO_Part_Material::UpdateConstantBuffer() const
{
    MaterialWrapper::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}

/*void WMO_Part_Material::fillRenderState(RenderState* _state) const
{
	uint16 sampler = m_QualitySettings.Texture_Sampler;
	sampler |= (m_Proto.flags.TextureClampS) ? SS_ADDRU_CLAMP : SS_ADDRU_WRAP;
	sampler |= (m_Proto.flags.TextureClampT) ? SS_ADDRV_CLAMP : SS_ADDRV_WRAP;

	_state->setTexture(Material::C_DiffuseTextureIndex + 0, m_DiffuseTexture[0], sampler, 0);
	_state->setTexture(Material::C_DiffuseTextureIndex + 1, m_DiffuseTexture[1], sampler, 0);
	_state->setCullMode(m_Proto.flags.IsTwoSided ? RS_CULL_NONE : RS_CULL_BACK);
	_Render->getRenderStorage()->SetEGxBlend(_state, m_Proto.blendMode);
}*/

/*void WMO_Part_Material::set() const
{
	uint16 sampler = m_QualitySettings.Texture_Sampler;
	sampler |= (m_Proto.flags.TextureClampS) ? SS_ADDRU_CLAMP : SS_ADDRU_WRAP;
	sampler |= (m_Proto.flags.TextureClampT) ? SS_ADDRV_CLAMP : SS_ADDRV_WRAP;

	_Render->r.setTexture(Material::C_DiffuseTextureIndex + 0, m_DiffuseTexture[0], sampler, 0);
	_Render->r.setTexture(Material::C_DiffuseTextureIndex + 1, m_DiffuseTexture[1], sampler, 0);

	_Render->r.setCullMode(m_Proto.flags.IsTwoSided ? RS_CULL_NONE : RS_CULL_BACK);

	//_Render->r.setAlphaToCoverage(true);

	_Render->getRenderStorage()->SetEGxBlend(_Render->r.getState(), m_Proto.blendMode);
}*/
