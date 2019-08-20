#include "stdafx.h"

// General
#include "LiquidMaterial.h"

LiquidMaterial::LiquidMaterial() :
	MaterialWrapper(_RenderDevice->CreateMaterial())
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();
	CreateConstantBuffer(m_pProperties, sizeof(MaterialProperties));

	// CreateShaders
	std::shared_ptr<Shader> g_pVertexShader = _RenderDevice->CreateShader(
		Shader::VertexShader, "shaders_D3D/Liquid.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
	);
    g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<Shader> g_pPixelShader = _RenderDevice->CreateShader(
		Shader::PixelShader, "shaders_D3D/Liquid.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
	);

	// Create samplers
	std::shared_ptr<SamplerState> g_Sampler = _RenderDevice->CreateSamplerState();
	g_Sampler->SetFilter(SamplerState::MinFilter::MinLinear, SamplerState::MagFilter::MagLinear, SamplerState::MipFilter::MipLinear);
	g_Sampler->SetWrapMode(SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp);

	// Assign samplers
	SetSampler(0, g_Sampler);

	// This
	SetShader(Shader::VertexShader, g_pVertexShader);
	SetShader(Shader::PixelShader, g_pPixelShader);
}

LiquidMaterial::~LiquidMaterial()
{
	if (m_pProperties)
	{
		_aligned_free(m_pProperties);
		m_pProperties = nullptr;
	}
}

void LiquidMaterial::SetShallowAlpha(float value)
{
	m_pProperties->gShallowAlpha = value;
	MarkConstantBufferDirty();
}

void LiquidMaterial::SetDeepAlpha(float value)
{
	m_pProperties->gDeepAlpha = value;
	MarkConstantBufferDirty();
}

void LiquidMaterial::SetColorLight(vec3 value)
{
	m_pProperties->gColorLight = value;
	MarkConstantBufferDirty();
}

void LiquidMaterial::SetColorDark(vec3 value)
{
	m_pProperties->gColorDark = value;
	MarkConstantBufferDirty();
}

//--

void LiquidMaterial::UpdateConstantBuffer() const
{
	MaterialWrapper::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}