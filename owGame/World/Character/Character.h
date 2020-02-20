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

    CInet_CharacterTemplate& GetTemplate() { return m_Template; }
	const CInet_CharacterTemplate& GetTemplate() const { return m_Template; }
	std::vector<std::shared_ptr<CItem_VisualData>>& GetVisualItems() { return m_VisualItems;	}

	// Texture components helper
	std::shared_ptr<const CItem_VisualData> getItemTextureComponents(InventoryType::List _slot) const { return m_VisualItems[_slot]; }

	// IRenderable
	void Render3D();

	// Refreshers
	void RefreshItemVisualData();
	void RefreshTextures(const Character_SectionWrapper& SectionWrapper, std::shared_ptr<ITexture> SkinTexture);
	void RefreshMeshIDs(const Character_SectionWrapper& SectionWrapper);

private:
	CInet_CharacterTemplate			                    m_Template;
	std::vector<std::shared_ptr<CItem_VisualData>>  m_VisualItems;
};