#pragma once

#ifdef USE_M2_MODELS

// FORWARD BEGIN
class Character;
// FORWARD END

class ZN_API Character_SkinTextureBaker
{
private:
	struct SRGBColor
	{
		uint8 r, g, b, a;
	};

public:
	Character_SkinTextureBaker(const IBaseManager& BaseManager);

	std::shared_ptr<IImage> CreateCharacterSkinImage(const CInet_CharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> CreateCharacterSkinWithItemsImage(std::shared_ptr<IImage> CharacterSkinImage, const Character* Character) const;

private:
	void FillWithSkin(const std::unique_ptr<SRGBColor[]>& Pixels, std::shared_ptr<IImage> SkinImage, bool NeedInvertY) const;
	void FillPixels  (const std::unique_ptr<SRGBColor[]>& Pixels, DBC_CharComponent_Sections _type, std::shared_ptr<IImage> Image) const;

private:
	struct SSkinRegion
	{
		SSkinRegion(uint32 X, uint32 Y, uint32 Width, uint32 Height)
			: X(X)
			, Y(Y)
			, Width(Width)
			, Height(Height)
		{}

		uint32 X;
		uint32 Y;
		uint32 Width;
		uint32 Height;
	};
	std::unordered_map<DBC_CharComponent_Sections, SSkinRegion> m_Regions;

private:
	const IBaseManager& m_BaseManager;
};

#endif