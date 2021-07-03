#pragma once

#ifdef USE_M2_MODELS

#include "../Creature/Creature.h"
#include "../Items/CharacterVisualItem.h"

#include "Types/CharacterTemplate.h"
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

class ZN_API Character 
	: public Creature
{
public:
    Character(IScene& Scene, const std::shared_ptr<CM2>& M2Object);
    virtual ~Character();

    CInet_CharacterTemplate& Template() { return m_Template; }
	const CInet_CharacterTemplate& GetTemplate() const { return m_Template; }

	// Texture components helper
	std::shared_ptr<const CCharacterVisualItem> getItemTextureComponents(DBCItem_EInventoryItemSlot _slot) const { return m_VisualItems.at(_slot); }

	// Refreshers
	void RefreshItemVisualData();
	void Refresh_CreateSkinTexture(std::shared_ptr<IImage> BakedSkinImage);
	void Refresh_AddItemsToSkinTexture();
	void RefreshMeshIDs();

	// Mesh & textures provider
	virtual void setMeshEnabled(MeshIDType _type, uint32 _value);
	virtual bool isMeshEnabled(uint32 _index) const override;

	// ISceneNode
	void Initialize() override;

	// ILoadable
	bool Load() override;

private:
	CInet_CharacterTemplate			                                         m_Template;
	std::map<DBCItem_EInventoryItemSlot, std::shared_ptr<CCharacterVisualItem>>  m_VisualItems;
	uint32                                                                   m_MeshID[(size_t)MeshIDType::Count];

	bool                                                                    m_IsNPCBakedTexturePresent;
	std::shared_ptr<IImage>                                                 m_BackedSkinImage;

};

#endif