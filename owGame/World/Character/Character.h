#pragma once

#include "World/Creature/Creature.h"
#include "World/Items/Item_VisualData.h"

#include "Client/Templates/CharacterTemplate.h"
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

class Character 
	: public Creature
{
public:
    Character(const M2& M2Object);
    virtual ~Character();

    const CInet_CharacterTemplate& GetTemplate() const { return m_Template; }

	// Initialization
	void InitFromTemplate(const CInet_CharacterTemplate& b);
	//void InitFromDisplayInfo(uint32 _id) override;
	void InitFromDisplayInfoCreating(uint32 _id, Race::List _race, Gender::List _gender);

	// Mesh provider
	void setHairGeoset(uint32 _value) { setMeshEnabled(MeshIDType::SkinAndHair, _value); }
	void setFacial1Geoset(uint32 _value) { setMeshEnabled(MeshIDType::Facial1, _value); }
	void setFacial2Geoset(uint32 _value) { setMeshEnabled(MeshIDType::Facial2, _value); }
	void setFacial3Geoset(uint32 _value) { setMeshEnabled(MeshIDType::Facial3, _value); }

	// Texture components helper
	std::shared_ptr<const CItem_VisualData> getItemTextureComponents(InventoryType::List _slot) const { return m_VisualItems[_slot]; }

	// IRenderable
	void Render3D();

	// Refreshers
	void RefreshItemVisualData();
	void RefreshTextures(const Character_SectionWrapper& SectionWrapper, const Character_SkinTextureBaker& SkinTextureBaker, std::shared_ptr<ITexture>& _skin);
	void RefreshMeshIDs(const Character_SectionWrapper& SectionWrapper);

private:
	CInet_CharacterTemplate			                    m_Template;
	std::vector<std::shared_ptr<CItem_VisualData>>  m_VisualItems;
};