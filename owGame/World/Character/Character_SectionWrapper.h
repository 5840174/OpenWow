#pragma once

#include "DBC/DBC__Storage.h"

// FORWARD BEGIN
class Character;
// FORWARD END

class Character_SectionWrapper
{
public:
	Character_SectionWrapper(IBaseManager& BaseManager, IRenderDevice& RenderDevice);
	virtual ~Character_SectionWrapper();

	// Skin
	std::shared_ptr<ITexture> getSkinTexture(const Character* Character) const;
	std::shared_ptr<ITexture> getSkinExtraTexture(const Character* Character) const;

	// Face
	std::shared_ptr<ITexture> getFaceLowerTexture(const Character* Character) const;
	std::shared_ptr<ITexture> getFaceUpperTexture(const Character* Character) const;

	// FacialHair
	std::string getFacialHairLowerTexture(const Character* Character) const;
	std::string getFacialHairUpperTexture(const Character* Character) const;
	uint32 getFacial01Geoset(const Character* Character) const;
	uint32 getFacial02Geoset(const Character* Character) const;
	uint32 getFacial03Geoset(const Character* Character) const;
	uint32 getFacial16Geoset(const Character* Character) const;
	uint32 getFacial17Geoset(const Character* Character) const;

	// Hair
	uint32     getHairGeoset(const Character* Character) const;
	uint32     getHairShowScalp(const Character* Character) const;
	std::shared_ptr<ITexture> getHairTexture(const Character* Character) const;
	std::shared_ptr<ITexture> getHairScalpLowerTexture(const Character* Character) const;
	std::shared_ptr<ITexture> getHairScalpUpperTexture(const Character* Character) const;

	// Naked
	std::string getNakedPelvisTexture(const Character* Character) const;
	std::string getNakedTorsoTexture(const Character* Character) const;

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	CDBCStorage* m_DBCs;
};