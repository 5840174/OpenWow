#pragma once

#ifdef USE_M2_MODELS

#include "../Creature/Creature.h"
#include "Types/CharacterTemplate.h"
#include "../Items/CharacterItem.h"

#include "Types/CharacterTemplate.h"

class ZN_API CCharacter 
	: public CCreature
{
public:
    CCharacter(IScene& Scene, const std::shared_ptr<CM2>& M2Object);
    virtual ~CCharacter();

	// CCharacter
    SCharacterTemplate& Template() { return m_Template; }
	const SCharacterTemplate& GetTemplate() const { return m_Template; }

	void SetNPCBakedImage(std::shared_ptr<IImage> BakedNPCImage);

	void Refresh_CharacterItemsFromTemplate();
	void Refresh_SkinImageFromTemplate();
	void Refresh_SkinWithItemsImage();
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
	void OnLoaded() override;

private:
	SCharacterTemplate			                     m_Template;
	std::shared_ptr<CCharacterItem>                  m_CharacterItems[INVENTORY_SLOT_BAG_END];
	uint32                                           m_MeshID[(size_t)EM2GeosetType::Count];

	bool                                             m_IsNPCBakedTexturePresent;
	std::shared_ptr<IImage>                          m_BackedSkinImage;

};

#endif