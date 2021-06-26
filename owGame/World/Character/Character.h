#pragma once

#ifdef USE_M2_MODELS

#include "World/Creature/Creature.h"
#include "World/Items/Item_VisualData.h"

#include "Client/Templates/CharacterTemplate.h"
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

class ZN_API Character 
	: public Creature
{
public:
    Character(IScene& Scene, const std::shared_ptr<CM2>& M2Object);
    virtual ~Character();

    CInet_CharacterTemplate& GetTemplate() { return m_Template; }
	const CInet_CharacterTemplate& GetTemplate() const { return m_Template; }
	const std::map<EInventoryType, std::shared_ptr<CItem_VisualData>>& GetVisualItems() { return m_VisualItems; }

	// Texture components helper
	std::shared_ptr<const CItem_VisualData> getItemTextureComponents(EInventoryType _slot) const { return m_VisualItems.at(_slot); }

	// Refreshers
	void RefreshItemVisualData();
	void RefreshTextures(const Character_SectionWrapper& SectionWrapper, std::shared_ptr<ITexture> SkinTexture);
	void RefreshMeshIDs(const Character_SectionWrapper& SectionWrapper);

	// ISceneNode
	void Initialize() override;

private:
	CInet_CharacterTemplate			                             m_Template;
	std::map<EInventoryType, std::shared_ptr<CItem_VisualData>>  m_VisualItems;
};

#endif