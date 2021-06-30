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
public:
	CM2_Comp_Materials(const CM2& M2Object);
	virtual ~CM2_Comp_Materials();

	void Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File);

	bool IsAnimTextures() const;

public:
	std::shared_ptr<const CM2_Part_Color>					GetColorDirect(uint32 Index) const;
	std::shared_ptr<const CM2_Part_Material>				GetMaterialDirect(uint32 Index) const;

	std::shared_ptr<const CM2_Part_Texture>					GetTextureDirectInternal(uint32 Index) const; // Used in Emitters
	std::shared_ptr<const CM2_Part_Texture>					GetTexture(uint32 Index) const;
	int16													GetTextureUnit(uint32 Index) const;
	std::shared_ptr<const CM2_Part_TextureWeight>			GetTextureWeight(uint32 Index) const;
	int16													GetTextureWeightLookup(uint32 Index) const;
	std::shared_ptr<const CM2_Part_TextureTransform>		GetTextureTransform(uint32 Index) const;
	int16													GetTextureTransformLookup(uint32 Index) const;
	int16													GetTextureCombiner(uint32 Index) const;

private:
	std::vector<std::shared_ptr<CM2_Part_Color>>			m_Colors;
	std::vector<std::shared_ptr<CM2_Part_Material>>			m_Materials;

	std::vector<std::shared_ptr<CM2_Part_Texture>>			m_Textures;
	std::vector<int16>										m_TexturesLookup;
	std::vector<int16>										m_ReplacebleLookup;    // index is TextureType, value is texture number
	std::vector<int16>										m_TexturesUnitLookup;
	std::vector<std::shared_ptr<CM2_Part_TextureWeight>>	m_TextureWeights;
	std::vector<int16>										m_TextureWeightsLookup;
	std::vector<std::shared_ptr<CM2_Part_TextureTransform>>	m_TexturesTransform;
	std::vector<int16>										m_TexturesTransformLookup;
	std::vector<uint16>										m_TexturesCombinerLookup;

private:
	bool													m_IsAnimTextures;

private:
	const CM2&												m_M2Object;
};
