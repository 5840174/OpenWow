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
	const uint32 M2COMBINER_OPAQUE   = 0;
	const uint32 M2COMBINER_MOD      = 1;
	const uint32 M2COMBINER_DECAL    = 2;
	const uint32 M2COMBINER_ADD      = 3;
	const uint32 M2COMBINER_MOD2X    = 4;
	const uint32 M2COMBINER_FADE     = 5;
	const uint32 M2COMBINER_MOD2X_NA = 6;
	const uint32 M2COMBINER_ADD_NA   = 7;
	const uint32 M2COMBINER_OP_MASK  = 7;
	const uint32 M2COMBINER_ENVMAP   = 8;

	/*const uint32 M2COMBINER_OPAQUE   = 0x00;
	const uint32 M2COMBINER_MOD      = 0x01;
	const uint32 M2COMBINER_DECAL    = 0x02;
	const uint32 M2COMBINER_ADD      = 0x04;
	const uint32 M2COMBINER_MOD2X    = 0x08;
	const uint32 M2COMBINER_FADE     = 0x10;
	const uint32 M2COMBINER_MOD2X_NA = 0x20;
	const uint32 M2COMBINER_ADD_NA   = 0x40;
	const uint32 M2COMBINER_OP_MASK  = 0x40;
	const uint32 M2COMBINER_ENVMAP   = 0x80;*/
}


CM2_Skin_Batch::CM2_Skin_Batch(const IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2& M2Model, const SM2_SkinBatch& SkinBatchProto)
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("CM2_Skin_Batch"))
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
	, m_SkinBatchProto(SkinBatchProto)
{
	// Shader ID
	m_ShaderID = SkinBatchProto.shader_id;

	// Get classes
	m_M2ModelMaterial = m_M2Model.getMaterials().GetMaterialDirect(SkinBatchProto.materialIndex);

	// Color
	if (SkinBatchProto.colorIndex != -1)
		m_Color = m_M2Model.getMaterials().GetColorDirect(SkinBatchProto.colorIndex);

	// Textures
	//if (SkinBatchProto.textureCount > 1)
	//	Log::Print("===============================");

	for (uint32 i = 0; i < SkinBatchProto.textureCount; i++)
	{
			//Log::Print("Batch texture '%d':", i);

			//if (SkinBatchProto.textureCoordComboIndex != -1)
			//	Log::Print("   Coord '%d'", m_M2Model.getMaterials().GetTextureUnit(SkinBatchProto.textureCoordComboIndex + i));
			//if (SkinBatchProto.textureTransformComboIndex != -1)
			//	Log::Print("   Transform '%d'", m_M2Model.getMaterials().GetTextureTransformLookup(SkinBatchProto.textureTransformComboIndex + i));

			//if (i == 1 && SkinBatchProto.textureTransformComboIndex != -1 && m_M2Model.getMaterials().GetTextureTransformLookup(SkinBatchProto.textureTransformComboIndex + i) != -1)
			//{
			//	Log::Warn("ModelName = '%s'", m_M2Model.getFilename().c_str());
			//	Log::Warn("FINDED!");
			//}

			//Log::Print("-------------------------");

		m_Textures.push_back(m_M2Model.getMaterials().GetTexture(SkinBatchProto.textureComboIndex + i));
	}

	if (m_Textures.size() > 2)
		throw CException("Model has more then 2 textures.");
	

	// Texture unit
	if (SkinBatchProto.textureCoordComboIndex != -1)
		m_TextureUnit = m_M2Model.getMaterials().GetTextureUnit(SkinBatchProto.textureCoordComboIndex);

	// Texture weight
	if (SkinBatchProto.textureWeightComboIndex != -1)
		m_TextureWeight = (m_M2Model.getMaterials().GetTextureWeight(SkinBatchProto.textureWeightComboIndex));

	// Texture transfowm
	if ((false == SkinBatchProto.flags.TextureStatic) && SkinBatchProto.textureTransformComboIndex != -1)
		m_TextureTransform = m_M2Model.getMaterials().GetTextureTransform(SkinBatchProto.textureTransformComboIndex);

	FixMyShaderID();
	if (m_ShaderID != 0)
	{
		//Log::Error("Shader = '%d'. '%s'. TexturesCnt = '%d'. Blend mode = '%d'.", m_ShaderID, M2GetPixelShaderName(SkinBatchProto.textureCount, m_ShaderID), SkinBatchProto.textureCount, m_M2ModelMaterial->getBlendMode());
	}
	else
	{
		//Log::Warn("Shader = '%d'. '%s'. TexturesCnt = '%d'. Blend mode = '%d'.", m_ShaderID, M2GetPixelShaderName(SkinBatchProto.textureCount, m_ShaderID), SkinBatchProto.textureCount, m_M2ModelMaterial->getBlendMode());
	}

	//Log::Error("ConvShader = '%d'", M2GetPixelShaderID(SkinBatchProto.textureCount, m_ShaderID));
	m_ShaderID = GetMyShaderID(M2GetPixelShaderID(SkinBatchProto.textureCount, m_ShaderID));


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
	auto& materialData = MaterialDataWithoutMark();

	// Shader
	materialData.gShader = m_ShaderID;

	// Blend mode
	materialData.gBlendMode = m_M2ModelMaterial->getBlendMode();

	// Model color
	if (m_Color != nullptr)
		materialData.gMaterialColorAndAlpha = m_Color->GetColorAndAlpha(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	// Texture alpha
	if (m_TextureWeight != nullptr)
		materialData.gTextureWeight = m_TextureWeight->GetWeight(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	// Texture transform
	materialData.gTextureAnimEnable = (m_TextureTransform != nullptr);
	if ((m_TextureTransform != nullptr))
		materialData.gTextureAnimMatrix = m_TextureTransform->GetTransform(m2Instance, static_cast<uint32>(RenderEventArgs.TotalTime));

	MarkMaterialDataDirty();

	// Textures
	for (size_t i = 0; i < m_SkinBatchProto.textureCount; i++)
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

void CM2_Skin_Batch::FixMyShaderID()
{
	if (m_ShaderID & 0x8000)
		return;

	auto material = m_M2ModelMaterial;

	// Flag 0x08: use_combiner_combos
	if (m_M2Model.GetHeader().global_flags.flag_use_texture_combiner_combos)
	{
		uint32_t textureCombinerComboIndex = m_ShaderID;
		uint32_t textureCoordComboIndex = m_SkinBatchProto.textureCoordComboIndex;

		m_ShaderID = 0;

		uint16_t shader[2] = { 0, 0 };
		for (uint32_t textureIndex = 0; textureIndex < m_SkinBatchProto.textureCount; ++textureIndex)
		{
			bool isFirstTexture = (textureIndex == 0);
			bool isLastTexture = (textureIndex == m_SkinBatchProto.textureCount - 1);

			uint16_t textureCombiner;

			// If m2 is the first texture and the batch material's blending mode is opaque, override the combiner mode to opaque; 
			// otherwise, use the combiner mode from the combiner combos
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
			//if (textureCoord > 2u)
			//	shader[textureIndex] |= M2COMBINER_ENVMAP;

			// If m2 is the last texture and the texture coord is T2, enable bit 15
			if (isLastTexture && textureCoord == 1)
				m_ShaderID |= 0x4000u;
		}

		m_ShaderID |= shader[0] << 4 | shader[1];
	}
	else
	{
		uint16_t shader = 0;
		uint16_t textureCombiner;
		uint32_t textureCoordComboIndex = m_SkinBatchProto.textureCoordComboIndex;

		// If the material blend mode is opaque, force the combiner to opaque; otherwise, default combiner to mod
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
		//if (textureCoord > 2u)
		//	shader |= M2COMBINER_ENVMAP;

		// If the texture coord is T2, enable bit 15
		if (textureCoord == 1)
			m_ShaderID |= 0x4000;

		m_ShaderID |= shader << 4;
	}
}
