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
	std::shared_ptr<ITexture> getSkinTexture(const Character* _character) const;
	std::shared_ptr<ITexture> getSkinExtraTexture(const Character* _character) const;

	// Face
	std::shared_ptr<ITexture> getFaceLowerTexture(const Character* _character) const;
	std::shared_ptr<ITexture> getFaceUpperTexture(const Character* _character) const;

	// FacialHair
	std::string getFacialHairLowerTexture(const Character* _character) const;
	std::string getFacialHairUpperTexture(const Character* _character) const;
	uint32 getFacial1Geoset(const Character* _character) const;
	uint32 getFacial2Geoset(const Character* _character) const;
	uint32 getFacial3Geoset(const Character* _character) const;
    uint32 getFacial16Geoset(const Character* _character) const;
    uint32 getFacial17Geoset(const Character* _character) const;

	// Hair
	uint32     getHairGeoset(const Character* _character) const;
	std::shared_ptr<ITexture> getHairTexture(const Character* _character) const;
	std::shared_ptr<ITexture> getHairScalpLowerTexture(const Character* _character) const;
	std::shared_ptr<ITexture> getHairScalpUpperTexture(const Character* _character) const;

	// Naked
	std::string getNakedPelvisTexture(const Character* _character) const;
	std::string getNakedTorsoTexture(const Character* _character) const;

private:
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	CDBCStorage* m_DBCs;
};