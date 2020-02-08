#pragma once

// Colors and textures
#include "M2_Part_Color.h"
#include "M2_Part_Material.h"
#include "M2_Part_Texture.h"
#include "M2_Part_TextureTransform.h"
#include "M2_Part_TextureWeight.h"

// FORWARD BEGIN
class CM2_Builder;
// FORWARD END

class CM2_Comp_Materials
{
	friend class CM2_Builder;
public:
	CM2_Comp_Materials();
	virtual ~CM2_Comp_Materials();

	void calc(uint16 anim, uint32 time, uint32 globalTime);

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
	std::vector<std::shared_ptr<CM2_Part_Color>>				m_Colors;
	std::vector<std::shared_ptr<CM2_Part_Material>>			m_Materials;

	//--------------------------------------------------------------------

public:
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
public:
	std::vector<std::shared_ptr<CM2_Part_Texture>>			m_Textures;
	std::vector<int16_t>						m_TexturesLookup;
	//--
	std::vector<int16_t>						m_TexturesUnitLookup;
	std::vector<int16_t>						m_ReplacebleLookup;    // index is TextureType, value is texture number
	//--
	std::vector<std::shared_ptr<CM2_Part_TextureWeight>>		m_TextureWeights;
	std::vector<int16_t>						m_TextureWeightsLookup;
	//--
	std::vector<std::shared_ptr<CM2_Part_TextureTransform>>	m_TexturesTransform;
	std::vector<int16_t>						m_TexturesTransformLookup;
	bool								m_IsAnimTextures;

};