#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Skin_Batch.h"

CM2_Skin_Batch::CM2_Skin_Batch(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2& M2Model, const SM2_SkinBatch& SkinBatchProto)
	: MaterialProxieT(RenderDevice, "CM2_Skin_Batch")
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
	, m_SkinBatchProto(SkinBatchProto)
{
	// Shader ID
	newShader = SkinBatchProto.shader_id;
	//Log::Info("Shader = '%d'", newShader);
	//if (newShader == -1)
	//	_ASSERT(false);

	// Geometry data
	m_PriorityPlan = SkinBatchProto.priorityPlane;

	// Get classes
	m_M2ModelMaterial = (m_M2Model.getMaterials().GetMaterialDirect(SkinBatchProto.materialIndex));

	// Color
	if (SkinBatchProto.colorIndex != -1)
	{
		m_Color = (m_M2Model.getMaterials().GetColorDirect(SkinBatchProto.colorIndex));
	}

	// Textures
	for (uint32 i = 0; i < SkinBatchProto.textureCount; i++)
	{
		m_Textures.push_back((m_M2Model.getMaterials().GetTexture(SkinBatchProto.texture_Index + i)));
	}
	_ASSERT(m_Textures.size() >= 0 && m_Textures.size() <= 2);
	if (m_Textures.size() == 2)
		newShader = 1;

	// Texture unit
	//if (SkinBatchProto.texture_CoordIndex != -1)
	//{
	//	m_TextureUnit = m_M2Model.getMaterials().m_TexturesUnitLookup[SkinBatchProto.texture_CoordIndex];
	//}

	// Texture weight
	if (SkinBatchProto.texture_WeightIndex != -1)
	{
		m_TextureWeight = (m_M2Model.getMaterials().GetTextureWeight(SkinBatchProto.texture_WeightIndex));
	}

	// Texture transfowm
	if (SkinBatchProto.flags.TextureStatic == false)
	{
		if (SkinBatchProto.texture_TransformIndex != -1)
		{
			m_TextureTransform = m_M2Model.getMaterials().GetTextureTransform(SkinBatchProto.texture_TransformIndex);
		}
	}

	// Samplers
	std::shared_ptr<ISamplerState> sampler = RenderDevice.GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	SetSampler(0, sampler);

	sampler = RenderDevice.GetObjectsFactory().CreateSamplerState();
	sampler->SetFilter(ISamplerState::MinFilter::MinLinear, ISamplerState::MagFilter::MagLinear, ISamplerState::MipFilter::MipLinear);
	SetSampler(1, sampler);
}

CM2_Skin_Batch::~CM2_Skin_Batch()
{
}

void CM2_Skin_Batch::UpdateMaterialProps(const RenderEventArgs& RenderEventArgs, const CM2_Base_Instance* m2Instance)
{
	ShaderM2BatchProperties props;

	// Shader
	props.gShader = newShader;

	// Blend mode
	props.gBlendMode = m_M2ModelMaterial->getBlendMode();

	// Model color
	if (m_Color != nullptr)
		props.gColor = m_Color->GetColorAndAlpha(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	// Texture alpha
	if (m_TextureWeight != nullptr)
		props.gTextureWeight = m_TextureWeight->GetWeight(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	// Texture transform
	props.gTextureAnimEnable = m_TextureTransform != nullptr;
	if (m_TextureTransform != nullptr)
		props.gTextureAnimMatrix = m_TextureTransform->GetTransform(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	// Instance color (for WMO doodads)
	props.gInstanceColor = m2Instance->getColor();

	// Textures
	//_ASSERT(m_Textures.size() < 2);
	for (uint32 i = 0; i < m_Textures.size(); i++)
	{
		std::shared_ptr<const CM2_Part_Texture> m2Texture = m_Textures[i].lock();
		if (m2Texture != nullptr)
		{
			SetTexture(i, m2Texture->GetTexture(m2Instance));
			GetSampler(i)->SetWrapMode(m2Texture->GetTextureWrapX(), m2Texture->GetTextureWrapY());
		}
		else
		{
			SetTexture(i, m_RenderDevice.GetDefaultTexture());
		}
	}

	MaterialProxie::UpdateConstantBuffer(&props, sizeof(ShaderM2BatchProperties));
}

#endif
