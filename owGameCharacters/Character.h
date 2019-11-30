#pragma once

#include "Creature.h"

#include "CharacterTemplate.h"

#include "Item_VisualData.h"

class Character : public Creature
{
public:
    Character();
    virtual ~Character();

    const CharacterTemplate& GetTemplate() const { return m_Template; }

	// Initialization
	void InitFromTemplate(const CharacterTemplate& b);
	void InitFromDisplayInfo(uint32 _id) override;
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

private:
	void CreateCharacterModel();

	// Refreshers
	void RefreshItemVisualData();
	void RefreshTextures(std::shared_ptr<ITexture> _skin = nullptr);
	void RefreshMeshIDs();

private:
	CharacterTemplate			                    m_Template;
	std::vector<std::shared_ptr<CItem_VisualData>>  m_VisualItems;
};