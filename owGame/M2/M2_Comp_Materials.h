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
class M2;
class CM2_Skin_Batch;
// FORWARD END

class CM2_Comp_Materials
{
	friend CM2_Skin_Batch;
public:
	CM2_Comp_Materials(const M2& M2Object);
	virtual ~CM2_Comp_Materials();

	void Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File);

	bool IsAnimTextures() const { return m_IsAnimTextures; }

public:
	std::shared_ptr<const CM2_Part_Color> GetColor(uint32 _index) const
	{
		_ASSERT(_index < m_Colors.size());
		return (m_Colors[_index]);
	}
	std::shared_ptr<const CM2_Part_Material> GetMaterial(uint32 _index) const
	{
		_ASSERT(_index < m_Materials.size());
		return (m_Materials[_index]);
	}
private:
	std::vector<std::shared_ptr<CM2_Part_Color>>     m_Colors;
	std::vector<std::shared_ptr<CM2_Part_Material>>  m_Materials;


public:
	std::shared_ptr<CM2_Part_Texture> GetTextureDirectInternal(uint32 _index) const // Used in Emitters
	{
		_ASSERT(_index < static_cast<uint32>(m_Textures.size()));
		return m_Textures[_index];
	}
	std::shared_ptr<CM2_Part_Texture> GetTexture(uint32 _index) const
	{
		_ASSERT(_index < m_TexturesLookup.size());
		if (_index >= m_TexturesLookup.size())
		{
			//Log::Warn("M2[%s]: GetTexture [%d] not found in Lookup[%d]", m_FileName.c_str(), _index, m_TexturesLookup.size());
			return nullptr;
		}
		int16 newIndex = m_TexturesLookup[_index];
		_ASSERT(newIndex != -1);
		_ASSERT(newIndex < static_cast<int16>(m_Textures.size()));
		return (m_Textures[newIndex]);
	}
	std::shared_ptr<CM2_Part_TextureWeight> GetTextureWeight(uint32 _index) const
	{
		if (_index >= m_TextureWeightsLookup.size())
		{
			//Log::Warn("M2[%s]: GetTextureWeight [%d] not found in Lookup[%d]", m_FileName.c_str(), _index, m_TextureWeightsLookup.size());
			return nullptr;
		}
		int16 newIndex = m_TextureWeightsLookup[_index];
		_ASSERT(newIndex != -1);
		_ASSERT(newIndex < static_cast<int16>(m_TextureWeights.size()));
		return (m_TextureWeights[newIndex]);
	}
	std::shared_ptr<CM2_Part_TextureTransform> GetTextureTransform(uint32 _index) const
	{
		if (_index >= m_TexturesTransformLookup.size())
		{
			//Log::Warn("M2[%s]: GetTextureTransform [%d] not found in Lookup[%d]", m_FileName.c_str(), _index, m_TexturesTransformLookup.size());
			return nullptr;
		}
		int16 newIndex = m_TexturesTransformLookup[_index];
		_ASSERT(newIndex != -1);
		_ASSERT(newIndex < static_cast<int16>(m_TexturesTransform.size()));
		return (m_TexturesTransform[newIndex]);
	}



private:
	std::vector<std::shared_ptr<CM2_Part_Texture>>           m_Textures;
	std::vector<int16_t>                                     m_TexturesLookup;
	//--
	std::vector<int16_t>                                     m_TexturesUnitLookup;
	std::vector<int16_t>                                     m_ReplacebleLookup;    // index is TextureType, value is texture number
	//--
	std::vector<std::shared_ptr<CM2_Part_TextureWeight>>     m_TextureWeights;
	std::vector<int16_t>                                     m_TextureWeightsLookup;
	//--
	std::vector<std::shared_ptr<CM2_Part_TextureTransform>>  m_TexturesTransform;
	std::vector<int16_t>                                     m_TexturesTransformLookup;

private:
	bool                                                     m_IsAnimTextures;

private:
	const M2& m_M2Object;
};