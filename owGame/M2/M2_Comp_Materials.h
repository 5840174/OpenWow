#pragma once

// M2
#include "M2_Headers.h"

// M2 materials
#include "M2_Part_Color.h"
#include "M2_Part_Material.h"
#include "M2_Part_Texture.h"
#include "M2_Part_TextureTransform.h"
#include "M2_Part_TextureWeight.h"

// FORWARD BEGIN
class CM2;
class CM2_Skin_Batch;
// FORWARD END

class CM2_Comp_Materials
{
	friend CM2_Skin_Batch;
public:
	CM2_Comp_Materials(const CM2& M2Object);
	virtual ~CM2_Comp_Materials();

	void Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File);

	bool IsAnimTextures() const { return m_IsAnimTextures; }

public:
	std::shared_ptr<const CM2_Part_Color> GetColorDirect(uint32 _index) const
	{
		_ASSERT(_index < m_Colors.size());
		return (m_Colors[_index]);
	}
	std::shared_ptr<const CM2_Part_Material> GetMaterialDirect(uint32 _index) const
	{
		_ASSERT(_index < m_Materials.size());
		return (m_Materials[_index]);
	}
private:
	std::vector<std::shared_ptr<CM2_Part_Color>>     m_Colors;
	std::vector<std::shared_ptr<CM2_Part_Material>>  m_Materials;


public:
	std::shared_ptr<const CM2_Part_Texture> GetTextureDirectInternal(uint32 _index) const // Used in Emitters
	{
		_ASSERT(_index < static_cast<uint32>(m_Textures.size()));
		return m_Textures[_index];
	}
	std::shared_ptr<const CM2_Part_Texture> GetTexture(uint32 _index) const
	{
		if (_index >= m_TexturesLookup.size())
			throw CException("GetTexture lookup index out of bounds.");

		int16 newIndex = m_TexturesLookup[_index];
		if (newIndex == -1)
			return nullptr;

		if (newIndex >= static_cast<int16>(m_Textures.size()))
			throw CException("GetTexture direct index out of bounds.");

		return (m_Textures[newIndex]);
	}
	int16 GetTextureUnit(uint32 _index) const
	{
		if (_index >= static_cast<uint32>(m_TexturesUnitLookup.size()))
			throw CException("GetTextureUnit direct index out of bounds.");

		return m_TexturesUnitLookup[_index];
	}
	std::shared_ptr<const CM2_Part_TextureWeight> GetTextureWeight(uint32 _index) const
	{
		if (_index >= m_TextureWeightsLookup.size())
			throw CException("GetTextureWeight lookup index out of bounds.");

		int16 newIndex = m_TextureWeightsLookup[_index];
		if (newIndex == -1)
			return nullptr;

		if (newIndex >= static_cast<int16>(m_TextureWeights.size()))
			throw CException("GetTextureWeight direct index out of bounds.");

		return m_TextureWeights[newIndex];
	}
	std::shared_ptr<const CM2_Part_TextureTransform> GetTextureTransform(uint32 _index) const
	{
		if (_index >= m_TexturesTransformLookup.size())
			throw CException("GetTextureTransform lookup index out of bounds.");

		int16 directIndex = m_TexturesTransformLookup[_index];
		if (directIndex == -1)
			return nullptr;

		if (directIndex >= static_cast<int16>(m_TexturesTransform.size()))
			throw CException("GetTextureTransform direct index out of bounds.");

		return m_TexturesTransform[directIndex];
	}
	int16 GetTextureCombiner(uint32 _index) const
	{
		if (_index >= static_cast<uint32>(m_TexturesCombinerLookup.size()))
			throw CException("GetTextureCombiner direct index out of bounds.");

		return m_TexturesCombinerLookup[_index];
	}


private:
	std::vector<std::shared_ptr<CM2_Part_Texture>>         m_Textures;
	std::vector<int16>                                     m_TexturesLookup;
	//--
	std::vector<int16>                                     m_ReplacebleLookup;    // index is TextureType, value is texture number
	std::vector<int16>                                     m_TexturesUnitLookup;
	//--
	std::vector<std::shared_ptr<CM2_Part_TextureWeight>>   m_TextureWeights;
	std::vector<int16>                                     m_TextureWeightsLookup;
	//--
	std::vector<std::shared_ptr<CM2_Part_TextureTransform>>m_TexturesTransform;
	std::vector<int16>                                     m_TexturesTransformLookup;
	//--
	std::vector<uint16>                                    m_TexturesCombinerLookup;

private:
	bool                                                   m_IsAnimTextures;

private:
	const CM2& m_M2Object;
};
