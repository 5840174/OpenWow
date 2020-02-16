#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"
#include "M2_Skin_Builder.h"

// General
#include "M2_Skin_Batch.h"

CM2_Skin_Batch::CM2_Skin_Batch(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const M2& M2Model, const SM2_SkinBatch& SkinBatchProto)
	: MaterialProxie(RenderDevice.GetObjectsFactory().CreateMaterial(sizeof(ShaderM2BatchProperties)))
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
	, m_SkinBatchProto(SkinBatchProto)
{
	m_Properties = (ShaderM2BatchProperties*)_aligned_malloc(sizeof(ShaderM2BatchProperties), 16);
	(*m_Properties) = ShaderM2BatchProperties();

	SetWrapper(this);

	// Shader ID
	newShader = SkinBatchProto.shader_id;

	// Geometry data
	m_PriorityPlan = SkinBatchProto.priorityPlane;

	// Get classes
	m_M2ModelMaterial = (m_M2Model.getMaterials()->GetMaterial(SkinBatchProto.materialIndex));

	// Color
	if (SkinBatchProto.colorIndex != -1)
	{
		m_Color = (m_M2Model.getMaterials()->GetColor(SkinBatchProto.colorIndex));
	}

	// Textures
	for (uint32 i = 0; i < SkinBatchProto.textureCount; i++)
	{
		m_Textures.push_back((m_M2Model.getMaterials()->GetTexture(SkinBatchProto.texture_Index + i)));
	}

	// Texture unit
	if (SkinBatchProto.texture_CoordIndex != -1)
	{
		m_TextureUnit = m_M2Model.getMaterials()->m_TexturesUnitLookup[SkinBatchProto.texture_CoordIndex];
	}

	// Texture weight
	if (SkinBatchProto.texture_WeightIndex != -1)
	{
		m_TextureWeight = (m_M2Model.getMaterials()->m_TextureWeights[SkinBatchProto.texture_WeightIndex]);
	}

	// Texture transfowm
	if (SkinBatchProto.flags.TextureStatic == false)
	{
		if (SkinBatchProto.texture_TransformIndex != -1)
		{
			int16 index = m_M2Model.getMaterials()->m_TexturesTransformLookup[SkinBatchProto.texture_TransformIndex];
			if (index != -1)
			{
				m_TextureTransform = (m_M2Model.getMaterials()->GetTextureTransform(SkinBatchProto.texture_TransformIndex));
			}
		}
	}

	// Samplers
	std::shared_ptr<ISamplerState> sampler = RenderDevice.GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	SetSampler(0, sampler);
	SetSampler(1, sampler);
}

CM2_Skin_Batch::~CM2_Skin_Batch()
{
	_aligned_free(m_Properties);
}

void CM2_Skin_Batch::UpdateMaterialProps(const CM2_Base_Instance* m2Instance)
{
	// Shader
	m_Properties->gShader = /*newShader*/0;

	// Blend mode
	m_Properties->gBlendMode = m_M2ModelMaterial->getBlendMode();

	// Model color
	bool isColorEnable = (m_Color != nullptr);
	m_Properties->gColorEnable = isColorEnable;
	if (isColorEnable)
	{
		m_Properties->gColor = m_Color->getValue();
	}

	// Texture alpha
	bool isTextureWeightEnable = (m_TextureWeight != nullptr);
	m_Properties->gTextureWeightEnable = isTextureWeightEnable;
	if (isTextureWeightEnable)
	{
		m_Properties->gTextureWeight = m_TextureWeight->getValue();
	}

	// Texture transform
	bool isTextureTransformEnable = (m_TextureTransform != nullptr);
	m_Properties->gTextureAnimEnable = isTextureTransformEnable;
	if (isTextureTransformEnable)
	{
		m_Properties->gTextureAnimMatrix = m_TextureTransform->getValue();
	}


	// Textures
	for (uint32 i = 0; i < m_Textures.size(); i++)
	{
		std::shared_ptr<const CM2_Part_Texture> m2Texture = m_Textures[i].lock();
		if (m2Texture == nullptr)
		{
			continue;
		}

		SetTexture(i, m2Texture->GetResultTexture(m2Instance));

		GetSampler(i)->SetWrapMode(m2Texture->GetTextureWrapX(), m2Texture->GetTextureWrapY());
	}


	MarkConstantBufferDirty();
}

void CM2_Skin_Batch::UpdateConstantBuffer() const
{
	MaterialProxie::UpdateConstantBuffer(m_Properties, sizeof(ShaderM2BatchProperties));
}
