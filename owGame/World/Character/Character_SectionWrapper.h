#pragma once

#ifdef USE_M2_MODELS

// FORWARD BEGIN
class CCharacter;
// FORWARD END

class ZN_API Character_SectionWrapper
{
public:
	Character_SectionWrapper(const IBaseManager& BaseManager);
	virtual ~Character_SectionWrapper();

	// Skin
	std::shared_ptr<IImage> getSkinTexture(const SCharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getSkinExtraTexture(const SCharacterTemplate& CharacterTemlate) const;

	// Face
	std::shared_ptr<IImage> getFaceLowerTexture(const SCharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getFaceUpperTexture(const SCharacterTemplate& CharacterTemlate) const;

	// FacialHair
	std::shared_ptr<IImage> getFacialHairLowerTexture(const SCharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getFacialHairUpperTexture(const SCharacterTemplate& CharacterTemlate) const;
	uint32 getFacial01Geoset(const SCharacterTemplate& CharacterTemlate) const;
	uint32 getFacial02Geoset(const SCharacterTemplate& CharacterTemlate) const;
	uint32 getFacial03Geoset(const SCharacterTemplate& CharacterTemlate) const;
	uint32 getFacial16Geoset(const SCharacterTemplate& CharacterTemlate) const;
	uint32 getFacial17Geoset(const SCharacterTemplate& CharacterTemlate) const;

	// Hair
	uint32     getHairGeoset(const SCharacterTemplate& CharacterTemlate) const;
	uint32     getHairShowScalp(const SCharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairTexture(const SCharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairScalpLowerTexture(const SCharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairScalpUpperTexture(const SCharacterTemplate& CharacterTemlate) const;

	// Naked
	std::string getNakedPelvisTexture(const SCharacterTemplate& CharacterTemlate) const;
	std::string getNakedTorsoTexture(const SCharacterTemplate& CharacterTemlate) const;

private:
	const IBaseManager& m_BaseManager;
	IFilesManager* m_FilesManager;
	CDBCStorage* m_DBCs;
};

#endif
