#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "Character.h"

// General
#include "Character_SkinTextureBaker.h"

// Additional
#include "Character_SectionWrapper.h"

namespace
{
const DBC_CharComponentTextureLayouts_Layout cSkinDefaultLayout = DBC_CharComponentTextureLayouts_Layout::LAYOUT_1;
const uint32                                 cSkinTextureWidth = 512;
const uint32                                 cSkinTextureHeight = 512;
const uint32                                 cSkinComponentWidth = cSkinTextureWidth / 2;
}

Character_SkinTextureBaker::Character_SkinTextureBaker(const IBaseManager& BaseManager)
	: m_BaseManager(BaseManager)
{
	auto dbcs = m_BaseManager.GetManager<CDBCStorage>();

	uint32 textureWidth = dbcs->DBC_CharComponentTextureLayouts()[(uint32)cSkinDefaultLayout]->Get_Width();
	uint32 textureHeight = dbcs->DBC_CharComponentTextureLayouts()[(uint32)cSkinDefaultLayout]->Get_Height();
	_ASSERT(textureWidth == cSkinTextureWidth);
	_ASSERT(textureHeight == cSkinTextureHeight);

	for (const auto& it : dbcs->DBC_CharComponentTextureSections())
	{
		const DBC_CharComponentTextureLayoutsRecord* layout = dbcs->DBC_CharComponentTextureLayouts()[it->Get_Layout()];
		if (layout->Get_ID() == (uint32)cSkinDefaultLayout)
		{
			m_Regions.insert(std::make_pair((DBC_CharComponent_Sections)it->Get_Section(), SSkinRegion(it->Get_X(), it->Get_Y(), it->Get_Width(), it->Get_Height())));
		}
	}
}

std::shared_ptr<IImage> Character_SkinTextureBaker::CreateCharacterSkinImage(const CInet_CharacterTemplate& CharacterTemlate) const
{
	std::unique_ptr<SRGBColor[]> pixels = std::unique_ptr<SRGBColor[]>(new SRGBColor[cSkinTextureWidth * cSkinTextureHeight]);
	std::memset(pixels.get(), 0x00, sizeof(SRGBColor) * cSkinTextureWidth * cSkinTextureHeight);

	Character_SectionWrapper sectionSrapper(m_BaseManager);

	// 1. Get skin texture as pattern
	{
		FillWithSkin(pixels, sectionSrapper.getSkinTexture(CharacterTemlate), true);
	}

	// 2. Hide boobs :)
	{
		// Female
		std::string nakedUpperTextureFileName = sectionSrapper.getNakedTorsoTexture(CharacterTemlate);
		if (nakedUpperTextureFileName.length() > 0)
			FillPixels(pixels, DBC_CharComponent_Sections::TORSO_UPPER, m_BaseManager.GetManager<IImagesFactory>()->CreateImage(nakedUpperTextureFileName));

		// Male + Female
		std::string nakedLowerTextureFileName = sectionSrapper.getNakedPelvisTexture(CharacterTemlate);
		if (nakedLowerTextureFileName.empty())
			throw CException("Character_SkinTextureBaker::CreateCharacterSkinImage: NakedPelvisTexture '%s' not found.", nakedLowerTextureFileName.c_str());

		FillPixels(pixels, DBC_CharComponent_Sections::LEGS_UPPER, m_BaseManager.GetManager<IImagesFactory>()->CreateImage(nakedLowerTextureFileName));
	}

	// Face
	{
		auto faceLowerIamge = sectionSrapper.getFaceLowerTexture(CharacterTemlate);
		if (faceLowerIamge)
			FillPixels(pixels, DBC_CharComponent_Sections::SCALP_LOWER, faceLowerIamge);

		auto faceUpperIamge = sectionSrapper.getFaceUpperTexture(CharacterTemlate);
		if (faceUpperIamge)
			FillPixels(pixels, DBC_CharComponent_Sections::SCALP_UPPER, faceUpperIamge);
	}

	// Facial hair
	{
		auto facialHairLowerIamge = sectionSrapper.getFacialHairLowerTexture(CharacterTemlate);
		if (facialHairLowerIamge)
			FillPixels(pixels, DBC_CharComponent_Sections::SCALP_LOWER, facialHairLowerIamge);

		auto facialHairUpperIamge = sectionSrapper.getFacialHairUpperTexture(CharacterTemlate);
		if (facialHairUpperIamge)
			FillPixels(pixels, DBC_CharComponent_Sections::SCALP_UPPER, facialHairUpperIamge);
	}

	// Hair scalp
	{
		auto hairScalpLowerIamge = sectionSrapper.getHairScalpLowerTexture(CharacterTemlate);
		if (hairScalpLowerIamge)
			FillPixels(pixels, DBC_CharComponent_Sections::SCALP_LOWER, hairScalpLowerIamge);

		auto hairScalpUpperIamge = sectionSrapper.getHairScalpUpperTexture(CharacterTemlate);
		if (hairScalpUpperIamge)
			FillPixels(pixels, DBC_CharComponent_Sections::SCALP_UPPER, hairScalpUpperIamge);
	}

	// 4. Final
	auto image = MakeShared(CImageBase, "", cSkinTextureWidth, cSkinTextureHeight, 32, true);
	std::memmove(image->GetDataEx(), pixels.get(), image->GetHeight() * image->GetStride());
	return image;
}

std::shared_ptr<IImage> Character_SkinTextureBaker::CreateCharacterSkinWithItemsImage(std::shared_ptr<IImage> CharacterSkinImage, const Character * Character) const
{
	std::unique_ptr<SRGBColor[]> pixels = std::unique_ptr<SRGBColor[]>(new SRGBColor[cSkinTextureWidth * cSkinTextureHeight]);
	std::memset(pixels.get(), 0x00, sizeof(SRGBColor) * cSkinTextureWidth * cSkinTextureHeight);

	// Set existing skin
	FillWithSkin(pixels, CharacterSkinImage, false);

	for (uint32 inventorySlot = 0; inventorySlot < INVENTORY_SLOT_BAG_END; inventorySlot++)
	{
		for (uint32 comp = 0; comp < static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT); comp++)
		{
			auto itemComponentTexture = Character->getItemTextureComponents(static_cast<DBCItem_EInventoryItemSlot>(inventorySlot))->getTextureComponent((DBC_CharComponent_Sections)comp);
			if (itemComponentTexture == nullptr)
				continue;

			FillPixels(pixels, (DBC_CharComponent_Sections)comp, itemComponentTexture);
		}
	}

	auto image = MakeShared(CImageBase, "", cSkinTextureWidth, cSkinTextureHeight, 32, true);
	std::memmove(image->GetDataEx(), pixels.get(), image->GetHeight() * image->GetStride());
	return image;
}

void Character_SkinTextureBaker::FillWithSkin(const std::unique_ptr<SRGBColor[]>& Pixels, std::shared_ptr<IImage> SkinImage, bool NeedInvertY) const
{
	_ASSERT(SkinImage->GetWidth() == (cSkinTextureWidth / 2) || SkinImage->GetWidth() == cSkinTextureWidth);

	const SRGBColor* skinTexturePixels = (const SRGBColor*)(SkinImage->GetData());

	_ASSERT(cSkinTextureWidth >= SkinImage->GetWidth());
	uint32 divSmall = cSkinTextureWidth / SkinImage->GetWidth();

	for (uint32 x = 0; x < cSkinTextureWidth; x++)
	{
		for (uint32 y = 0; y < cSkinTextureHeight; y++)
		{
			uint32 yCoord = NeedInvertY ? (cSkinTextureHeight - y - 1) : y;

			uint32 index      = ( x             + (yCoord         * cSkinTextureWidth));
			uint32 indexLocal = ((x / divSmall) + ((y / divSmall) * SkinImage->GetWidth()));
			if (skinTexturePixels[indexLocal].a > 224) // 224.0f / 255.0f
				Pixels[index] = skinTexturePixels[indexLocal];
		}
	}
}
void Character_SkinTextureBaker::FillPixels(const std::unique_ptr<SRGBColor[]>& Pixels, DBC_CharComponent_Sections ComponentType, std::shared_ptr<IImage> ComponentImage) const
{
	_ASSERT(ComponentImage->GetWidth() == 128 || ComponentImage->GetWidth() == 256);

	const SRGBColor* texturePixels = (const SRGBColor*)ComponentImage->GetData();

	_ASSERT(cSkinComponentWidth >= ComponentImage->GetWidth());
	uint32 divSmall = cSkinComponentWidth / ComponentImage->GetWidth();

	const auto& region = m_Regions.at(ComponentType);
	for (uint32 x = 0; x < region.Width; x++)
	{
		for (uint32 y = 0; y < region.Height; y++)
		{
			const uint32 xWithRegion = (region.X + x);
			const uint32 yWithRegion = (region.Y + y);

			uint32 index      = xWithRegion     + ((cSkinTextureHeight - yWithRegion - 1) * cSkinTextureWidth);
			uint32 indexLocal = ((x / divSmall) + ((y / divSmall)                         * ComponentImage->GetWidth()));

			if (texturePixels[indexLocal].a > 224) // 224.0f / 255.0f
				Pixels[index] = texturePixels[indexLocal];
		}
	}
}

#endif
