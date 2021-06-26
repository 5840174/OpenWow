#pragma once

#ifdef USE_M2_MODELS

#include "DBC/DBC__Storage.h"

// FORWARD BEGIN
class Character;
// FORWARD END

class Character_SectionWrapper
{
public:
	Character_SectionWrapper(const IBaseManager& BaseManager);
	virtual ~Character_SectionWrapper();

	// Skin
	std::shared_ptr<IImage> getSkinTexture(const CInet_CharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getSkinExtraTexture(const CInet_CharacterTemplate& CharacterTemlate) const;

	// Face
	std::shared_ptr<IImage> getFaceLowerTexture(const CInet_CharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getFaceUpperTexture(const CInet_CharacterTemplate& CharacterTemlate) const;

	// FacialHair
	std::string getFacialHairLowerTexture(const CInet_CharacterTemplate& CharacterTemlate) const;
	std::string getFacialHairUpperTexture(const CInet_CharacterTemplate& CharacterTemlate) const;
	uint32 getFacial01Geoset(const CInet_CharacterTemplate& CharacterTemlate) const;
	uint32 getFacial02Geoset(const CInet_CharacterTemplate& CharacterTemlate) const;
	uint32 getFacial03Geoset(const CInet_CharacterTemplate& CharacterTemlate) const;
	uint32 getFacial16Geoset(const CInet_CharacterTemplate& CharacterTemlate) const;
	uint32 getFacial17Geoset(const CInet_CharacterTemplate& CharacterTemlate) const;

	// Hair
	uint32     getHairGeoset(const CInet_CharacterTemplate& CharacterTemlate) const;
	uint32     getHairShowScalp(const CInet_CharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairTexture(const CInet_CharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairScalpLowerTexture(const CInet_CharacterTemplate& CharacterTemlate) const;
	std::shared_ptr<IImage> getHairScalpUpperTexture(const CInet_CharacterTemplate& CharacterTemlate) const;

	// Naked
	std::string getNakedPelvisTexture(const CInet_CharacterTemplate& CharacterTemlate) const;
	std::string getNakedTorsoTexture(const CInet_CharacterTemplate& CharacterTemlate) const;

private:
	const IBaseManager& m_BaseManager;
	CDBCStorage* m_DBCs;
};

#endif
