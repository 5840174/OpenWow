#pragma once

// FORWARD BEGIN
class Character;
// FORWARD END

class Character_SectionWrapper
{
public:
	// Skin
	static std::shared_ptr<ITexture> getSkinTexture(const Character* _character);
	static std::shared_ptr<ITexture> getSkinExtraTexture(const Character* _character);

	// Face
	static std::shared_ptr<ITexture> getFaceLowerTexture(const Character* _character);
	static std::shared_ptr<ITexture> getFaceUpperTexture(const Character* _character);

	// FacialHair
	static std::string getFacialHairLowerTexture(const Character* _character);
	static std::string getFacialHairUpperTexture(const Character* _character);
	static uint32 getFacial1Geoset(const Character* _character);
	static uint32 getFacial2Geoset(const Character* _character);
	static uint32 getFacial3Geoset(const Character* _character);
    static uint32 getFacial16Geoset(const Character* _character);
    static uint32 getFacial17Geoset(const Character* _character);

	// Hair
	static uint32     getHairGeoset(const Character* _character);
	static std::shared_ptr<ITexture> getHairTexture(const Character* _character);
	static std::shared_ptr<ITexture> getHairScalpLowerTexture(const Character* _character);
	static std::shared_ptr<ITexture> getHairScalpUpperTexture(const Character* _character);

	// Naked
	static std::string getNakedPelvisTexture(const Character* _character);
	static std::string getNakedTorsoTexture(const Character* _character);
};