#include "stdafx.h"

// Include
#include "Character.h"

// General
#include "Character_SkinTextureBaker.h"

// Additional
#include "Character_SectionWrapper.h"

const CharacterSkinLayout::List  SkinDefaultLayout = CharacterSkinLayout::LAYOUT_1;
const uint32                     SkinTextureWidth = 512;
const uint32                     SkinTextureHeight = 512;
const uint32                     SkinComponentWidth = SkinTextureWidth / 2;

Character_SkinTextureBaker::Character_SkinTextureBaker()
{
	uint32 textureWidth = DBC_CharComponentTextureLayouts[SkinDefaultLayout]->Get_Width();
	uint32 textureHeight = DBC_CharComponentTextureLayouts[SkinDefaultLayout]->Get_Height();
	assert1(textureWidth == SkinTextureWidth);
	assert1(textureHeight == SkinTextureHeight);

	for (auto& it : DBC_CharComponentTextureSections)
	{
		if (it->Get_Layout()->Get_ID() == SkinDefaultLayout)
		{
			CharacterSkinRegion region;
			region.X = it->Get_X();
			region.Y = it->Get_Y();
			region.Width = it->Get_Width();
			region.Height = it->Get_Height();
			m_Regions.insert(std::make_pair((DBC_CharComponent_Sections::List)it->Get_Section(), region));
		}
	}
}

std::shared_ptr<Texture> Character_SkinTextureBaker::createTexture(const Character* _character)
{
	std::shared_ptr<Texture> bakedSkinTexture = _RenderDevice->CreateTexture();

	m_Pixels = new PixelData[SkinTextureWidth * SkinTextureHeight];
	memset(m_Pixels, 0x00, sizeof(PixelData) * SkinTextureWidth * SkinTextureHeight);

	// 1. Get skin texture as pattern
	{
		FillWithSkin(Character_SectionWrapper::getSkinTexture(_character));
	}

	// 2. Hide boobs :)
	{
		// Female
		std::string nakedUpperTexture = Character_SectionWrapper::getNakedTorsoTexture(_character);
		if (nakedUpperTexture.length() > 0)
			FillPixels(DBC_CharComponent_Sections::TORSO_UPPER, nakedUpperTexture);

		// Male + Female
		std::string nakedLowerTexture = Character_SectionWrapper::getNakedPelvisTexture(_character);
		assert1(nakedLowerTexture.length() > 0);
		FillPixels(DBC_CharComponent_Sections::LEGS_UPPER, nakedLowerTexture);
	}

	// 3. Apply items texture components
	{
		for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++)
		{
			for (uint32 comp = 0; comp < DBC_CharComponent_Sections::ITEMS_COUNT; comp++)
			{
				std::shared_ptr<Texture> itemComponentTexture = _character->getItemTextureComponents((InventoryType::List)slot)->getTextureComponent((DBC_CharComponent_Sections::List)comp);
				if (itemComponentTexture == nullptr)
					continue;

				FillPixels((DBC_CharComponent_Sections::List) comp, itemComponentTexture);
			}
		}
	}

	// 4. Final
	bakedSkinTexture->LoadTextureCustom(SkinTextureWidth, SkinTextureHeight, m_Pixels);
	SafeDeleteArray(m_Pixels);

	return bakedSkinTexture;
}

void Character_SkinTextureBaker::FillWithSkin(std::shared_ptr<Texture> _skinTexture)
{
	assert1(_skinTexture != nullptr);
	assert1(_skinTexture->GetWidth() == (SkinTextureWidth / 2) || _skinTexture->GetWidth() == SkinTextureWidth);

	const PixelData* skinTexturePixels = (const PixelData*)(_skinTexture->GetBuffer().data());

	assert1(SkinTextureWidth >= _skinTexture->GetWidth());
	uint32 divSmall = SkinTextureWidth / _skinTexture->GetWidth();

	for (uint32 x = 0; x < SkinTextureWidth; x++)
	{
		for (uint32 y = 0; y < SkinTextureWidth; y++)
		{
			uint32 index = (x + (y * SkinTextureWidth));
			uint32 indexLocal = ((x / divSmall) + ((y / divSmall) * _skinTexture->GetWidth()));
			m_Pixels[index] = skinTexturePixels[indexLocal];
		}
	}
}

void Character_SkinTextureBaker::FillPixels(DBC_CharComponent_Sections::List _type, std::string _name)
{
	std::shared_ptr<Texture> texture = _RenderDevice->CreateTexture2D(_name);
	if (texture == nullptr)
		return;

	FillPixels(_type, texture);
}

void Character_SkinTextureBaker::FillPixels(DBC_CharComponent_Sections::List _type, std::shared_ptr<Texture> _compTexture)
{
	if (_compTexture == nullptr)
		return;

	assert1(_compTexture->GetWidth() == 128 || _compTexture->GetWidth() == 256);

	const PixelData* texturePixels = (const PixelData*)_compTexture->GetBuffer().data();

	assert1(SkinComponentWidth >= _compTexture->GetWidth());
	uint32 divSmall = SkinComponentWidth / _compTexture->GetWidth();

	CharacterSkinRegion& region = m_Regions[_type];
	for (uint32 x = 0; x < region.Width; x++)
	{
		for (uint32 y = 0; y < region.Height; y++)
		{
			uint32 index = (region.X + x) + ((region.Y + y) * SkinTextureWidth);
			uint32 indexLocal = ((x / divSmall) + ((y / divSmall) * _compTexture->GetWidth()));

			if (texturePixels[indexLocal].a > 0)
			{
				m_Pixels[index] = texturePixels[indexLocal];
			}
		}
	}
}
