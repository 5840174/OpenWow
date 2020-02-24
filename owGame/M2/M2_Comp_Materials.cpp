#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Comp_Materials.h"

CM2_Comp_Materials::CM2_Comp_Materials(const M2& M2Object) 
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
			if (m2Colors[i].color.interpolation_type != Interpolations::INTERPOLATION_NONE || m2Colors[i].alpha.interpolation_type != Interpolations::INTERPOLATION_NONE)
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
			std::shared_ptr<CM2_Part_Material> material = std::make_shared<CM2_Part_Material>(m_M2Object, m2Materials[i]);
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
	if (M2Header.textureLookup.size > 0)
	{
		int16* m2TexturesLookups = (int16*)(File->getData() + M2Header.textureLookup.offset);
		for (uint32 i = 0; i < M2Header.textureLookup.size; i++)
		{
			m_TexturesLookup.push_back(m2TexturesLookups[i]);
		}
	}

	// 3.3 Textures unit lookup
	if (M2Header.textureUnitLookup.size > 0)
	{
		int16* m2TexturesUnitLookups = (int16*)(File->getData() + M2Header.textureUnitLookup.offset);
		for (uint32 i = 0; i < M2Header.textureUnitLookup.size; i++)
		{
			m_TexturesUnitLookup.push_back(m2TexturesUnitLookups[i]);
		}
	}

	// 3.4 Replaceble textures lookup
	if (M2Header.replacable_texture_lookup.size > 0)
	{
		int16* m2ReplacebleLookup = (int16*)(File->getData() + M2Header.replacable_texture_lookup.offset);
		for (uint32 i = 0; i < M2Header.replacable_texture_lookup.size; i++)
		{
			m_ReplacebleLookup.push_back(m2ReplacebleLookup[i]);
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
			if (m2TexturesWeights[i].weight.interpolation_type != Interpolations::INTERPOLATION_NONE)
			{
				//m_M2->m_HasMisc = true;
			}
		}
	}

	// 4.2 Textures weights lookup
	if (M2Header.textureWeightsLookup.size > 0)
	{
		int16* m2TextureWeightsLookup = (int16*)(File->getData() + M2Header.textureWeightsLookup.offset);
		for (uint32 i = 0; i < M2Header.textureWeightsLookup.size; i++)
		{
			m_TextureWeightsLookup.push_back(m2TextureWeightsLookup[i]);
		}
	}

	//_ASSERT(M2Header.textureWeights.size == M2Header.textureWeightsLookup.size);

	// 5.1 Textures transform
	if (M2Header.textureTransforms.size > 0)
	{
		SM2_TextureTransform* m2TexturesTransforms = (SM2_TextureTransform*)(File->getData() + M2Header.textureTransforms.offset);
		for (uint32 i = 0; i < M2Header.textureTransforms.size; i++)
		{
			std::shared_ptr<CM2_Part_TextureTransform> textureTransform = std::make_shared<CM2_Part_TextureTransform>(m_M2Object, File, m2TexturesTransforms[i]);
			m_TexturesTransform.push_back(textureTransform);

			// AnimTextures
			if (m2TexturesTransforms[i].translation.interpolation_type != Interpolations::INTERPOLATION_NONE ||
				m2TexturesTransforms[i].rotation.interpolation_type != Interpolations::INTERPOLATION_NONE ||
				m2TexturesTransforms[i].scaling.interpolation_type != Interpolations::INTERPOLATION_NONE)
			{
				m_IsAnimTextures = true;
			}
		}
	}

	// 5.2 Textures transform lookup
	if (M2Header.textureTransformsLookup.size > 0)
	{
		int16* m2TextureTransformsLookups = (int16*)(File->getData() + M2Header.textureTransformsLookup.offset);
		for (uint32 i = 0; i < M2Header.textureTransformsLookup.size; i++)
		{
			m_TexturesTransformLookup.push_back(m2TextureTransformsLookups[i]);
		}
	}
}
