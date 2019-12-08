#pragma once

struct CharacterSkinLayout
{
	enum List : uint32
	{
		LAYOUT_1 = 1,
		LAYOUT_2 = 2
	};
};

// FORWARD BEGIN
class Character;
// FORWARD END

class Character_SkinTextureBaker
{
public:
	Character_SkinTextureBaker();

	std::shared_ptr<ITexture> createTexture(const Character* _character);

private:
	void FillWithSkin(std::shared_ptr<ITexture> _texture);
	void FillPixels(DBC_CharComponent_Sections::List _type, std::string _name);
	void FillPixels(DBC_CharComponent_Sections::List _type, std::shared_ptr<ITexture> _texture);

private:
	struct PixelData
	{
		uint8 r, g, b, a;
	};

	struct CharacterSkinRegion
	{
		uint32 X;
		uint32 Y;
		uint32 Width;
		uint32 Height;
	};

	PixelData*	                                              m_Pixels;
	std::unordered_map<DBC_CharComponent_Sections::List, CharacterSkinRegion> m_Regions;
};