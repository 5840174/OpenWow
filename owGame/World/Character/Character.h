#pragma once

#ifdef USE_M2_MODELS

#include "../Creature/Creature.h"
#include "../Items/Item_VisualData.h"

#include "Types/CharacterTemplate.h"
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
	const std::map<DBCItem_EInventoryType, std::shared_ptr<CItem_VisualData>>& GetVisualItems() { return m_VisualItems; }

	// Texture components helper
	std::shared_ptr<const CItem_VisualData> getItemTextureComponents(DBCItem_EInventoryType _slot) const { return m_VisualItems.at(_slot); }

	// Refreshers
	void RefreshItemVisualData();
	void RefreshTextures(const Character_SectionWrapper& SectionWrapper, std::shared_ptr<ITexture> SkinTexture);
	void RefreshMeshIDs(const Character_SectionWrapper& SectionWrapper);

	// Mesh & textures provider
	virtual void setMeshEnabled(MeshIDType _type, uint32 _value);
	virtual bool isMeshEnabled(uint32 _index) const override;

	// ISceneNode
	void Initialize() override;

private:
	CInet_CharacterTemplate			                             m_Template;
	std::map<DBCItem_EInventoryType, std::shared_ptr<CItem_VisualData>>  m_VisualItems;
	uint32                                                       m_MeshID[(size_t)MeshIDType::Count];
};

#endif