#pragma once

#ifdef USE_M2_MODELS

// FORWARD BEGIN
class Character;
// FORWARD END

class ZN_API Character_SkinTextureBaker
{
public:
	Character_SkinTextureBaker(const IBaseManager& BaseManager, IRenderDevice& RenderDevice);

	std::shared_ptr<ITexture> createTexture(const Character* _character) const;

private:
	void FillWithSkin(std::shared_ptr<IImage> Image) const;
	void FillPixels(DBC_CharComponent_Sections _type, std::shared_ptr<IImage> Image) const;

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
	const IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	CDBCStorage* m_DBCs;
};

#endif