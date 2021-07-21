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
	std::shared_ptr<IImage> getSkinTexture(const SCharacterVisualTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getSkinExtraTexture(const SCharacterVisualTemplate& CharacterTemlate) const;

	// Face
	std::shared_ptr<IImage> getFaceLowerTexture(const SCharacterVisualTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getFaceUpperTexture(const SCharacterVisualTemplate& CharacterTemlate) const;

	// FacialHair
	std::shared_ptr<IImage> getFacialHairLowerTexture(const SCharacterVisualTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getFacialHairUpperTexture(const SCharacterVisualTemplate& CharacterTemlate) const;
	uint32 getFacial01Geoset(const SCharacterVisualTemplate& CharacterTemlate) const;
	uint32 getFacial02Geoset(const SCharacterVisualTemplate& CharacterTemlate) const;
	uint32 getFacial03Geoset(const SCharacterVisualTemplate& CharacterTemlate) const;
	uint32 getFacial16Geoset(const SCharacterVisualTemplate& CharacterTemlate) const;
	uint32 getFacial17Geoset(const SCharacterVisualTemplate& CharacterTemlate) const;

	// Hair
	uint32     getHairGeoset(const SCharacterVisualTemplate& CharacterTemlate) const;
	uint32     getHairShowScalp(const SCharacterVisualTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairTexture(const SCharacterVisualTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairScalpLowerTexture(const SCharacterVisualTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairScalpUpperTexture(const SCharacterVisualTemplate& CharacterTemlate) const;

	// Naked
	std::string getNakedPelvisTexture(const SCharacterVisualTemplate& CharacterTemlate) const;
	std::string getNakedTorsoTexture(const SCharacterVisualTemplate& CharacterTemlate) const;

private:
	const IBaseManager& m_BaseManager;
	IFilesManager* m_FilesManager;
	CDBCStorage* m_DBCs;
};

#endif
