#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Skin_Batch.h"

// Additional
#include "ShaderResolver.h"

namespace
{
	/*const uint32 M2COMBINER_OPAQUE   = 0;
	const uint32 M2COMBINER_MOD      = 1;
	const uint32 M2COMBINER_DECAL    = 2;
	const uint32 M2COMBINER_ADD      = 3;
	const uint32 M2COMBINER_MOD2X    = 4;
	const uint32 M2COMBINER_FADE     = 5;
	const uint32 M2COMBINER_MOD2X_NA = 6;
	const uint32 M2COMBINER_ADD_NA   = 7;
	const uint32 M2COMBINER_OP_MASK  = 7;
	const uint32 M2COMBINER_ENVMAP   = 8;*/

	const uint32 M2COMBINER_OPAQUE   = 0x00;
	const uint32 M2COMBINER_MOD      = 0x01;
	const uint32 M2COMBINER_DECAL    = 0x02;
	const uint32 M2COMBINER_ADD      = 0x04;
	const uint32 M2COMBINER_MOD2X    = 0x08;
	const uint32 M2COMBINER_FADE     = 0x10;
	const uint32 M2COMBINER_MOD2X_NA = 0x20;
	const uint32 M2COMBINER_ADD_NA   = 0x40;
	const uint32 M2COMBINER_OP_MASK  = 0x40;
	const uint32 M2COMBINER_ENVMAP   = 0x80;
}


CM2_Skin_Batch::CM2_Skin_Batch(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2& M2Model, const SM2_SkinBatch& SkinBatchProto)
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("CM2_Skin_Batch"))
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
	, m_SkinBatchProto(SkinBatchProto)
{
	// Shader ID
	newShader = SkinBatchProto.shader_id;

	// Geometry data
	m_PriorityPlan = SkinBatchProto.priorityPlane;

	// Get classes
	m_M2ModelMaterial = m_M2Model.getMaterials().GetMaterialDirect(SkinBatchProto.materialIndex);

	// Color
	if (SkinBatchProto.colorIndex != -1)
	{
		m_Color = (m_M2Model.getMaterials().GetColorDirect(SkinBatchProto.colorIndex));
	}

	// Textures
	for (uint32 i = 0; i < SkinBatchProto.textureCount; i++)
	{
		m_Textures.push_back((m_M2Model.getMaterials().GetTexture(SkinBatchProto.textureComboIndex + i)));
	}
	_ASSERT(m_Textures.size() >= 0 && m_Textures.size() <= 2);
	if (m_Textures.size() == 2)
	{
		if (newShader >= 6 /*Combiners_Add_Add*/)
		{
			Log::Warn("M2 shader setted to 1 (old %d)", newShader);
			newShader = 1;
		}
	}

	// Texture unit
	//if (SkinBatchProto.texture_CoordIndex != -1)
	//{
	//	m_TextureUnit = m_M2Model.getMaterials().m_TexturesUnitLookup[SkinBatchProto.texture_CoordIndex];
	//}

	// Texture weight
	if (SkinBatchProto.textureWeightComboIndex != -1)
	{
		m_TextureWeight = (m_M2Model.getMaterials().GetTextureWeight(SkinBatchProto.textureWeightComboIndex));
	}

	// Texture transfowm
	if (SkinBatchProto.flags.TextureStatic == false)
	{
		if (SkinBatchProto.textureTransformComboIndex != -1)
		{
			m_TextureTransform = m_M2Model.getMaterials().GetTextureTransform(SkinBatchProto.textureTransformComboIndex);
		}
	}

	FixMyShaderID(SkinBatchProto);
	if (newShader != 0)
	{
		Log::Error("Shader = '%d'. '%s'. TexturesCnt = '%d'. Blend mode = '%d'.", newShader, M2GetPixelShaderName(SkinBatchProto.textureCount, newShader), SkinBatchProto.textureCount, m_M2ModelMaterial->getBlendMode());
	}
	else
	{
		Log::Warn("Shader = '%d'. '%s'. TexturesCnt = '%d'. Blend mode = '%d'.", newShader, M2GetPixelShaderName(SkinBatchProto.textureCount, newShader), SkinBatchProto.textureCount, m_M2ModelMaterial->getBlendMode());
	}

	Log::Error("ConvShader = '%d'", M2GetPixelShaderID(SkinBatchProto.textureCount, newShader));
	newShader = GetMyShaderID(M2GetPixelShaderID(SkinBatchProto.textureCount, newShader));
	//newShader = 11;


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
	// Shader
	MaterialData().gShader = newShader;

	// Blend mode
	MaterialData().gBlendMode = m_M2ModelMaterial->getBlendMode();

	// Model color
	if (m_Color != nullptr)
		MaterialData().gMaterialColorAndAlpha = m_Color->GetColorAndAlpha(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	// Texture alpha
	if (m_TextureWeight != nullptr)
		MaterialData().gTextureWeight = m_TextureWeight->GetWeight(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	// Texture transform
	MaterialData().gTextureAnimEnable = m_TextureTransform != nullptr;
	if (m_TextureTransform != nullptr)
		MaterialData().gTextureAnimMatrix = m_TextureTransform->GetTransform(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	// Instance color (for WMO doodads) we set it in M2PerObject
	// MaterialData().gInstanceColor = m2Instance->getColor();

	// Textures
	for (uint32 i = 0; i < m_Textures.size(); i++)
	{
		if (auto m2Texture = m_Textures[i].lock())
		{
			SetTexture(i, m2Texture->GetTexture(m2Instance));
			GetSampler(i)->SetWrapMode(m2Texture->GetTextureWrapX(), m2Texture->GetTextureWrapY());
		}
		else
		{
			SetTexture(i, m_RenderDevice.GetBaseManager().GetManager<IznTexturesFactory>()->GetDefaultTexture());
		}
	}
}

void CM2_Skin_Batch::FixMyShaderID(const SM2_SkinBatch& Batch)
{
	if (newShader & 0x8000)
		return;

	auto material = m_M2ModelMaterial;

	// Flag 0x08: use_combiner_combos
	if (m_M2Model.GetHeader().global_flags.flag_use_texture_combiner_combos)
	{
		uint32_t textureCombinerComboIndex = newShader;
		uint32_t textureCoordComboIndex = Batch.textureCoordComboIndex;

		newShader = 0;

		uint16_t shader[2] = { 0, 0 };
		for (uint32_t textureIndex = 0; textureIndex < Batch.textureCount; ++textureIndex)
		{
			bool isFirstTexture = (textureIndex == 0);
			bool isLastTexture = (textureIndex == Batch.textureCount - 1);

			uint16_t textureCombiner;

			// If m2 is the first texture and the batch material's blending mode is opaque,
			// override the combiner mode to opaque; otherwise, use the combiner mode from the
			// combiner combos
			if (isFirstTexture && (material->getBlendMode() == SM2_Material::M2BLEND_OPAQUE))
			{
				textureCombiner = M2COMBINER_OPAQUE;
			}
			else
			{
				textureCombiner = m_M2Model.getMaterials().GetTextureCombiner(textureCombinerComboIndex + textureIndex);
			}

			shader[textureIndex] = textureCombiner;

			// If the texture coord is env, set env bit for texture
			uint16_t textureCoord = m_M2Model.getMaterials().GetTextureUnit(textureCoordComboIndex + textureIndex);
			if (textureCoord > 2u)
				shader[textureIndex] |= M2COMBINER_ENVMAP;

			// If m2 is the last texture and the texture coord is T2, enable bit 15
			if (isLastTexture && textureCoord == 1)
				newShader |= 0x4000u;
		}

		newShader |= shader[0] << 4 | shader[1];
	}
	else
	{
		uint16_t shader = 0;
		uint16_t textureCombiner;
		uint32_t textureCoordComboIndex = Batch.textureCoordComboIndex;

		// If the material blend mode is opaque, force the combiner to opaque; otherwise,
		// default combiner to mod
		if (material->getBlendMode() == SM2_Material::M2BLEND_OPAQUE)
		{
			textureCombiner = M2COMBINER_OPAQUE;
		}
		else
		{
			textureCombiner = M2COMBINER_MOD;
		}

		shader = textureCombiner;

		// If the texture coord is env, set env bit for texture
		uint16_t textureCoord = m_M2Model.getMaterials().GetTextureUnit(textureCoordComboIndex);
		if (textureCoord > 2u)
			shader |= M2COMBINER_ENVMAP;

		// If the texture coord is T2, enable bit 15
		if (textureCoord == 1)
			newShader |= 0x4000;

		newShader |= shader << 4;
	}
}
