#pragma once

#ifdef USE_M2_MODELS

#include "../Creature/Creature.h"
#include "Types/CharacterTemplate.h"
#include "../Items/CharacterItem.h"

#include "Types/CharacterTemplate.h"

class ZN_API CCharacter 
	: public Creature
{
public:
    CCharacter(IScene& Scene, const std::shared_ptr<CM2>& M2Object);
    virtual ~CCharacter();

    SCharacterTemplate& Template() { return m_Template; }
	const SCharacterTemplate& GetTemplate() const { return m_Template; }

	// Refreshers
	void RefreshCharacterItemsFromTemplate();
	void Refresh_CreateSkinTexture(std::shared_ptr<IImage> BakedSkinImage);
	void Refresh_AddItemsToSkinTexture();
	void RefreshMeshIDs();

	// Items
	std::shared_ptr<CCharacterItem> GetItem(uint8 InventorySlot) const;
	void SetItem(uint8 InventorySlot, const SCharacterItemTemplate& ItemTemplate);

	// Geosets
	virtual void setMeshEnabled(EM2GeosetType M2GeosetType, uint32 _value);
	virtual bool isMeshEnabled(uint32 _index) const override;

	// ISceneNode
	void Initialize() override;

	// ILoadable
	bool Load() override;

private:
	SCharacterTemplate			                     m_Template;
	std::shared_ptr<CCharacterItem>                  m_CharacterItems[INVENTORY_SLOT_BAG_END];
	uint32                                           m_MeshID[(size_t)EM2GeosetType::Count];

	bool                                             m_IsNPCBakedTexturePresent;
	std::shared_ptr<IImage>                          m_BackedSkinImage;

};

#endif