#include "stdafx.h"

// General
#include "LiquidMaterial.h"

LiquidMaterial::LiquidMaterial(IBaseManager* BaseManager) 
	: MaterialProxie(GetManager<IRenderDevice>(BaseManager)->CreateMaterial(sizeof(MaterialProperties)))
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();



	// Create samplers
	std::shared_ptr<ISamplerState> g_Sampler = GetManager<IRenderDevice>(BaseManager)->CreateSamplerState();
	g_Sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	g_Sampler->SetWrapMode(ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp);

	// Assign samplers
	SetSampler(0, g_Sampler);


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