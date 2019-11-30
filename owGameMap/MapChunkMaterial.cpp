#include "stdafx.h"

// Include
#include "MapTile.h"

// General
#include "MapChunkMaterial.h"


ADT_MCNK_Material::ADT_MCNK_Material(const std::weak_ptr<CMapTile> _parentADT) :
	MaterialWrapper(_RenderDevice->CreateMaterial(sizeof(MaterialProperties)))
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();

	// Create samplers
	std::shared_ptr<ISamplerState> g_LinearClampSampler = _RenderDevice->CreateSamplerState();
	g_LinearClampSampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	g_LinearClampSampler->SetWrapMode(ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp, ISamplerState::WrapMode::Clamp);

	std::shared_ptr<ISamplerState> g_LinearRepeatSampler = _RenderDevice->CreateSamplerState();
	g_LinearRepeatSampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	g_LinearRepeatSampler->SetWrapMode(ISamplerState::WrapMode::Repeat, ISamplerState::WrapMode::Repeat, ISamplerState::WrapMode::Repeat);

    SetSampler(0, g_LinearRepeatSampler);
    SetSampler(1, g_LinearRepeatSampler);
    SetSampler(2, g_LinearRepeatSampler);
    SetSampler(3, g_LinearRepeatSampler);
    SetSampler(4, g_LinearClampSampler);
}

ADT_MCNK_Material::~ADT_MCNK_Material()
{
	if (m_pProperties)
	{
		_aligned_free(m_pProperties);
		m_pProperties = nullptr;
	}
}

//--

void ADT_MCNK_Material::SetLayersCnt(uint32 value)
{
	m_pProperties->m_LayersCnt = value;
	MarkConstantBufferDirty();
}

void ADT_MCNK_Material::SetShadowMapExists(uint32 value)
{
	m_pProperties->m_ShadowMapExists = value;
	MarkConstantBufferDirty();
}

//--

void ADT_MCNK_Material::UpdateConstantBuffer() const
{
	MaterialWrapper::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}
