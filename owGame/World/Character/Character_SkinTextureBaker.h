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
	Character_SkinTextureBaker(IBaseManager& BaseManager, IRenderDevice& RenderDevice);

	std::shared_ptr<ITexture> createTexture(const Character* _character) const;

private:
	void FillWithSkin(std::shared_ptr<ITexture> _texture) const;
	void FillPixels(DBC_CharComponent_Sections _type, std::string _name) const;
	void FillPixels(DBC_CharComponent_Sections _type, std::shared_ptr<ITexture> _texture) const;

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

	mutable PixelData*	                                                              m_Pixels;
	std::unordered_map<DBC_CharComponent_Sections, CharacterSkinRegion> m_Regions;

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	CDBCStorage* m_DBCs;
};