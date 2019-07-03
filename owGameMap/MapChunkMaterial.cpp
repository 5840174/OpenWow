#include "stdafx.h"

// Include
#include "MapTile.h"

// General
#include "MapChunkMaterial.h"


ADT_MCNK_Material::ADT_MCNK_Material(const std::weak_ptr<CMapTile> _parentADT) :
	MaterialWrapper(_RenderDevice->CreateMaterial())
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();
	CreateConstantBuffer(m_pProperties, sizeof(MaterialProperties));

    std::shared_ptr<Shader> g_pVertexShader;
    std::shared_ptr<Shader> g_pPixelShader;

    if (_RenderDevice->GetDeviceType() == IRenderDevice::DeviceType::DirectX)
    {
        g_pVertexShader = _RenderDevice->CreateShader(
            Shader::VertexShader, "shaders_D3D/MapChunk.hlsl", Shader::ShaderMacros(), "VS_main", "latest"
        );
        g_pPixelShader = _RenderDevice->CreateShader(
            Shader::PixelShader, "shaders_D3D/MapChunk.hlsl", Shader::ShaderMacros(), "PS_main", "latest"
        );

    }
    else if (_RenderDevice->GetDeviceType() == IRenderDevice::DeviceType::OpenGL)
    {
        g_pVertexShader = _RenderDevice->CreateShader(
            Shader::VertexShader, "shaders_OGL/MapChunk.vs", Shader::ShaderMacros(), "", ""
        );
        g_pPixelShader = _RenderDevice->CreateShader(
            Shader::PixelShader, "shaders_OGL/MapChunk.ps", Shader::ShaderMacros(), "", ""
        );
    }
    g_pVertexShader->LoadInputLayoutFromReflector();


	// Create samplers
	std::shared_ptr<SamplerState> g_LinearClampSampler = _RenderDevice->CreateSamplerState();
	g_LinearClampSampler->SetFilter(SamplerState::MinFilter::MinLinear, SamplerState::MagFilter::MagLinear, SamplerState::MipFilter::MipLinear);
	g_LinearClampSampler->SetWrapMode(SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp, SamplerState::WrapMode::Clamp);

	std::shared_ptr<SamplerState> g_LinearRepeatSampler = _RenderDevice->CreateSamplerState();
	g_LinearRepeatSampler->SetFilter(SamplerState::MinFilter::MinLinear, SamplerState::MagFilter::MagLinear, SamplerState::MipFilter::MipLinear);
	g_LinearRepeatSampler->SetWrapMode(SamplerState::WrapMode::Repeat, SamplerState::WrapMode::Repeat, SamplerState::WrapMode::Repeat);

    SetSampler(0, g_LinearRepeatSampler);
    SetSampler(1, g_LinearRepeatSampler);
    SetSampler(2, g_LinearRepeatSampler);
    SetSampler(3, g_LinearRepeatSampler);
    SetSampler(4, g_LinearClampSampler);

	SetShader(Shader::VertexShader, g_pVertexShader);
	SetShader(Shader::PixelShader, g_pPixelShader);
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
