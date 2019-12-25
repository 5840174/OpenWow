#include "stdafx.h"

// General
#include "M2_Material.h"

M2_Material::M2_Material(IBaseManager* BaseManager, std::vector<std::weak_ptr<const CM2_Part_Texture>> m2Textures)
	: MaterialProxie(BaseManager->GetManager<IRenderDevice>()->CreateMaterial(sizeof(MaterialProperties)))
{
	m_pProperties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
	*m_pProperties = MaterialProperties();
	memset(m_pProperties, 0x00, sizeof(MaterialProperties));

	std::shared_ptr<ISamplerState> g_Sampler = BaseManager->GetManager<IRenderDevice>()->CreateSamplerState();
	g_Sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);

	_ASSERT(m2Textures.size() <= 2);
	for (uint8 i = 0; i < m2Textures.size(); i++)
	{
        SetSampler(i, g_Sampler);
	}
}

M2_Material::~M2_Material()
{
	if (m_pProperties)
	{
		_aligned_free(m_pProperties);
		m_pProperties = nullptr;
	}
}

void M2_Material::SetAnimated(bool value)
{
	m_pProperties->gIsAnimated = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetColorEnable(bool value)
{
	m_pProperties->gColorEnable = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetColor(vec4 value)
{
	m_pProperties->gColor = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetBones(const std::vector<mat4>& bones)
{
	for (uint8 i = 0; i < bones.size(); i++)
	{
		m_pProperties->Bones[i] = bones[i];
	}
	MarkConstantBufferDirty();
}

void M2_Material::SetMaxInfluences(uint32 value)
{
	m_pProperties->gBonesMaxInfluences = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetBlendMode(uint32 value)
{
	m_pProperties->gBlendMode = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetNewShader(uint32 value)
{
	m_pProperties->gShader = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetTextureWeightEnable(bool value)
{
	m_pProperties->gTextureWeightEnable = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetTextureWeight(float value)
{
	m_pProperties->gTextureWeight = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetTextureAnimEnable(bool value)
{
	m_pProperties->gTextureAnimEnable = value;
	MarkConstantBufferDirty();
}

void M2_Material::SetTextureAnimMatrix(cmat4 value)
{
	m_pProperties->gTextureAnimMatrix = value;
	MarkConstantBufferDirty();
}

//--

void M2_Material::UpdateConstantBuffer() const
{
	MaterialProxie::UpdateConstantBuffer(m_pProperties, sizeof(MaterialProperties));
}