#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Comp_Materials.h"

CM2_Comp_Materials::CM2_Comp_Materials(const CM2& M2Object) 
	: m_IsAnimTextures(false)
	, m_M2Object(M2Object)
{}

CM2_Comp_Materials::~CM2_Comp_Materials()
{}


void CM2_Comp_Materials::Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File)
{
	// 1 Colors
	if (M2Header.colors.size > 0)
	{
		SM2_Color* m2Colors = (SM2_Color*)(File->getData() + M2Header.colors.offset);
		for (uint32 i = 0; i < M2Header.colors.size; i++)
		{
			std::shared_ptr<CM2_Part_Color> color = std::make_shared<CM2_Part_Color>(m_M2Object, File, m2Colors[i]);
			m_Colors.push_back(color);

			// Animated
			if (m2Colors[i].color.interpolation_type != Interpolations::None || m2Colors[i].alpha.interpolation_type != Interpolations::None)
			{
				//m_M2->m_HasMisc = true;
			}
		}
	}

	// 2 Materials
	if (M2Header.materials.size > 0)
	{
		SM2_Material* m2Materials = (SM2_Material*)(File->getData() + M2Header.materials.offset);
		for (uint32 i = 0; i < M2Header.materials.size; i++)
		{
			std::shared_ptr<CM2_Part_Material> material = std::make_shared<CM2_Part_Material>(m_M2Object.GetBaseManager(), m_M2Object.GetRenderDevice(), m_M2Object, m2Materials[i]);
			m_Materials.push_back(material);
		}
	}

	// 3.1 Textures
	if (M2Header.textures.size > 0)
	{
		_ASSERT(M2Header.textures.size <= cM2Model_TexturesMaxCount);

		SM2_Texture* m2Textures = (SM2_Texture*)(File->getData() + M2Header.textures.offset);
		for (uint32 i = 0; i < M2Header.textures.size; i++)
		{
			std::shared_ptr<CM2_Part_Texture> texture = std::make_shared<CM2_Part_Texture>(m_M2Object.GetBaseManager(), m_M2Object.GetRenderDevice(), m_M2Object, File, m2Textures[i]);
			m_Textures.push_back(texture);
		}
	}

	// 3.2 Textures lookup
	if (M2Header.textureCombos.size > 0)
	{
		int16* m2TexturesLookups = (int16*)(File->getData() + M2Header.textureCombos.offset);
		for (uint32 i = 0; i < M2Header.textureCombos.size; i++)
		{
			m_TexturesLookup.push_back(m2TexturesLookups[i]);
		}
	}

	// 3.3 Replaceble textures lookup
	if (M2Header.textureIndicesById.size > 0)
	{
		int16* m2ReplacebleLookup = (int16*)(File->getData() + M2Header.textureIndicesById.offset);
		for (uint32 i = 0; i < M2Header.textureIndicesById.size; i++)
			m_ReplacebleLookup.push_back(m2ReplacebleLookup[i]);
	}

	// 3.4 Textures unit lookup
	if (M2Header.textureCoordCombos.size > 0)
	{
		int16* m2TexturesUnitLookups = (int16*)(File->getData() + M2Header.textureCoordCombos.offset);
		for (uint32 i = 0; i < M2Header.textureCoordCombos.size; i++)
		{
			m_TexturesUnitLookup.push_back(m2TexturesUnitLookups[i]);
		}
	}

	// 4.1 Textures weights
	if (M2Header.textureWeights.size > 0)
	{
		SM2_TextureWeight* m2TexturesWeights = (SM2_TextureWeight*)(File->getData() + M2Header.textureWeights.offset);
		for (uint32 i = 0; i < M2Header.textureWeights.size; i++)
		{
			std::shared_ptr<CM2_Part_TextureWeight> textureWeight = std::make_shared<CM2_Part_TextureWeight>(m_M2Object, File, m2TexturesWeights[i]);
			m_TextureWeights.push_back(textureWeight);

			// Animated
			if (m2TexturesWeights[i].weight.interpolation_type != Interpolations::None)
			{
				//m_M2->m_HasMisc = true;
			}
		}
	}

	// 4.2 Textures weights lookup
	if (M2Header.textureWeightCombos.size > 0)
	{
		int16* m2TextureWeightsLookup = (int16*)(File->getData() + M2Header.textureWeightCombos.offset);
		for (uint32 i = 0; i < M2Header.textureWeightCombos.size; i++)
		{
			m_TextureWeightsLookup.push_back(m2TextureWeightsLookup[i]);
		}
	}

	// 5.1 Textures transform
	if (M2Header.textureTransforms.size > 0)
	{
		SM2_TextureTransform* m2TexturesTransforms = (SM2_TextureTransform*)(File->getData() + M2Header.textureTransforms.offset);
		for (uint32 i = 0; i < M2Header.textureTransforms.size; i++)
		{
			std::shared_ptr<CM2_Part_TextureTransform> textureTransform = std::make_shared<CM2_Part_TextureTransform>(m_M2Object, File, m2TexturesTransforms[i]);
			m_TexturesTransform.push_back(textureTransform);

			// AnimTextures
			if (m2TexturesTransforms[i].translation.interpolation_type != Interpolations::None ||
				m2TexturesTransforms[i].rotation.interpolation_type != Interpolations::None ||
				m2TexturesTransforms[i].scaling.interpolation_type != Interpolations::None)
			{
				m_IsAnimTextures = true;
			}
		}
	}

	// 5.2 Textures transform lookup
	if (M2Header.textureTransformCombos.size > 0)
	{
		int16* m2TextureTransformsLookups = (int16*)(File->getData() + M2Header.textureTransformCombos.offset);
		for (uint32 i = 0; i < M2Header.textureTransformCombos.size; i++)
		{
			m_TexturesTransformLookup.push_back(m2TextureTransformsLookups[i]);
		}
	}

	// 6.1 Textures combiner lookup
	if ((M2Header.global_flags.flag_use_texture_combiner_combos) && (M2Header.textureCombinerCombos.size > 0))
	{
		int16* m2TextureCombinerLookups = (int16*)(File->getData() + M2Header.textureCombinerCombos.offset);
		for (uint32 i = 0; i < M2Header.textureCombinerCombos.size; i++)
			m_TexturesCombinerLookup.push_back(m2TextureCombinerLookups[i]);
	}
}

bool CM2_Comp_Materials::IsAnimTextures() const
{
	return m_IsAnimTextures;
}



//
// Material & Colors
//
std::shared_ptr<const CM2_Part_Color> CM2_Comp_Materials::GetColorDirect(uint32 Index) const
{
	if (Index >= m_Colors.size())
		throw CException("GetColorDirect direct index out of bounds.");
	return (m_Colors[Index]);
}

std::shared_ptr<const CM2_Part_Material> CM2_Comp_Materials::GetMaterialDirect(uint32 Index) const
{
	if (Index >= m_Materials.size())
		throw CException("GetMaterialDirect direct index out of bounds.");
	return (m_Materials[Index]);
}



//
// Textures
//
std::shared_ptr<const CM2_Part_Texture> CM2_Comp_Materials::GetTextureDirectInternal(uint32 Index) const // Used in Emitters
{
	if (Index >= static_cast<uint32>(m_Textures.size()))
		throw CException("GetTextureDirectInternal direct index out of bounds.");
	return m_Textures[Index];
}

std::shared_ptr<const CM2_Part_Texture> CM2_Comp_Materials::GetTexture(uint32 Index) const
{
	if (Index >= m_TexturesLookup.size())
		throw CException("GetTexture lookup index out of bounds.");

	int16 newIndex = m_TexturesLookup[Index];
	if (newIndex == -1)
		return nullptr;

	if (newIndex >= static_cast<int16>(m_Textures.size()))
		throw CException("GetTexture direct index out of bounds.");

	return (m_Textures[newIndex]);
}

int16 CM2_Comp_Materials::GetTextureUnit(uint32 Index) const
{
	if (Index >= static_cast<uint32>(m_TexturesUnitLookup.size()))
		throw CException("GetTextureUnit direct index out of bounds.");

	return m_TexturesUnitLookup[Index];
}

std::shared_ptr<const CM2_Part_TextureWeight> CM2_Comp_Materials::GetTextureWeight(uint32 Index) const
{
	int16 newIndex = GetTextureWeightLookup(Index);
	if (newIndex == -1)
		return nullptr;

	if (newIndex >= static_cast<int16>(m_TextureWeights.size()))
		throw CException("GetTextureWeight direct index out of bounds.");

	return m_TextureWeights[newIndex];
}

int16 CM2_Comp_Materials::GetTextureWeightLookup(uint32 Index) const
{
	if (Index >= m_TextureWeightsLookup.size())
		throw CException("GetTextureWeightLookup lookup index out of bounds.");

	return m_TextureWeightsLookup[Index];
}

std::shared_ptr<const CM2_Part_TextureTransform> CM2_Comp_Materials::GetTextureTransform(uint32 Index) const
{
	int16 directIndex = GetTextureTransformLookup(Index);
	if (directIndex == -1)
		return nullptr;

	if (directIndex >= static_cast<int16>(m_TexturesTransform.size()))
		throw CException("GetTextureTransform direct index out of bounds.");

	return m_TexturesTransform[directIndex];
}

int16 CM2_Comp_Materials::GetTextureTransformLookup(uint32 Index) const
{
	if (Index >= m_TexturesTransformLookup.size())
		throw CException("GetTextureTransformLookup lookup index out of bounds.");

	return m_TexturesTransformLookup[Index];
}

int16 CM2_Comp_Materials::GetTextureCombiner(uint32 Index) const
{
	if (false == m_M2Object.GetHeader().global_flags.flag_use_texture_combiner_combos)
		throw CException("GetTextureCombiner model hasn't textures combiners.");

	if (Index >= static_cast<uint32>(m_TexturesCombinerLookup.size()))
		throw CException("GetTextureCombiner direct index out of bounds.");

	return m_TexturesCombinerLookup[Index];
}
