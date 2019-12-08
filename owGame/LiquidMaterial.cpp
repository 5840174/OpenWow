#include "stdafx.h"

// General
#include "LiquidMaterial.h"

LiquidMaterial::LiquidMaterial(IBaseManager* BaseManager) 
	: MaterialProxie(GetManager<IRenderDevice>(BaseManager)->CreateMaterial(sizeof(MaterialProperties)))
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();

	// CreateShaders
	std::shared_ptr<IShader> g_pVertexShader = GetManager<IRenderDevice>(BaseManager)->CreateShader(
		IShader::ShaderType::VertexShader, "shaders_D3D/Liquid.hlsl", IShader::ShaderMacros(), "VS_main", "latest"
	);
    g_pVertexShader->LoadInputLayoutFromReflector();

	std::shared_ptr<IShader> g_pPixelShader = GetManager<IRenderDevice>(BaseManager)->CreateShader(
		IShader::ShaderType::PixelShader, "shaders_D3D/Liquid.hlsl", IShader::ShaderMacros(), "PS_main", "latest"
	);

	// Create samplers
	std::shared_ptr<ISamplerState> g_Sampler = GetManager<IRenderDevice>(BaseManager)->CreateSamplerState();
	g_Sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	g_Sampler->SetWrapMode(ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp);

	// Assign samplers
	SetSampler(0, g_Sampler);

	// This
	SetShader(IShader::ShaderType::VertexShader, g_pVertexShader);
	SetShader(IShader::ShaderType::PixelShader, g_pPixelShader);
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
	MaterialProxie::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}