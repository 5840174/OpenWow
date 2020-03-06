#include "stdafx.h"

// General
#include "Character.h"

// Additional
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

Character::Character(const std::shared_ptr<CM2>& M2Object)
	: Creature(M2Object)
{
	setMeshEnabled(MeshIDType::Ears, EarsStyles::Enabled);
	setMeshEnabled(MeshIDType::Eyeglows, EyeglowsStyles::Racial);
}

Character::~Character()
{
}

void Character::Render3D()
{
	//CM2_Base_Instance::Render3D();

	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++)
	{
		if (slot == EQUIPMENT_SLOT_HEAD && (m_Template.Flags & CHARACTER_FLAG_HIDE_HELM))
		{
			continue;
		}

		m_VisualItems[slot]->Render3D();
	}
}





void Character::RefreshItemVisualData()
{
	// 3. Items visual data
	for (uint32 i = 0; i < INVENTORY_SLOT_BAG_END; i++)
	{
		m_VisualItems[i]->TemplateSet(m_Template.ItemsTemplates[i]);
		m_VisualItems[i]->Load();

		if (i == EQUIPMENT_SLOT_HEAD)
		{
			if ((m_Template.Flags & CHARACTER_FLAG_HIDE_HELM))
			{
				setMeshEnabled(MeshIDType::Ears, EarsStyles::Enabled);
				continue;
			}
			else
			{
				setMeshEnabled(MeshIDType::Ears, EarsStyles::None);
			}
		}
		else if (i == EQUIPMENT_SLOT_BACK)
		{
			if ((m_Template.Flags & CHARACTER_FLAG_HIDE_CLOAK))
			{
				setMeshEnabled(MeshIDType::Cloak, 1);
				continue;
			}
            else
            {

            }
		}

		for (auto& geoset : m_VisualItems[i]->getGeosetComponents())
		{
			setMeshEnabled(geoset.mesh, geoset.getMeshID());
		}
	}
}

void Character::RefreshTextures(const Character_SectionWrapper& SectionWrapper, std::shared_ptr<ITexture> SkinTexture)
{
	setSpecialTexture(SM2_Texture::Type::SKIN, SkinTexture);
	setSpecialTexture(SM2_Texture::Type::SKIN_EXTRA, SectionWrapper.getSkinExtraTexture(this));
	setSpecialTexture(SM2_Texture::Type::CHAR_HAIR, SectionWrapper.getHairTexture(this));

	// Cloak
	std::shared_ptr<const CItem_VisualData> item = m_VisualItems[EQUIPMENT_SLOT_BACK];
	if (item->InventoryType != InventoryType::NON_EQUIP)
	{
		_ASSERT(item->getObjectComponents().size() == 1);
		std::shared_ptr<ITexture> cloackTexttre = item->getObjectComponents()[0].texture;
		setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, cloackTexttre);
	}
}

void Character::RefreshMeshIDs(const Character_SectionWrapper& SectionWrapper)
{
	if (SectionWrapper.getHairShowScalp(this))
		setMeshEnabled(MeshIDType::SkinAndHair, 1);
	else
		setMeshEnabled(MeshIDType::SkinAndHair, SectionWrapper.getHairGeoset(this));

	setMeshEnabled(MeshIDType::Facial1, SectionWrapper.getFacial1Geoset(this));
	setMeshEnabled(MeshIDType::Facial2, (SectionWrapper.getFacial3Geoset(this) == 0) ? 1 : SectionWrapper.getFacial3Geoset(this));
	setMeshEnabled(MeshIDType::Facial3, (SectionWrapper.getFacial2Geoset(this) == 0) ? 1 : SectionWrapper.getFacial2Geoset(this));

    //setMeshEnabled(MeshIDType::Unk2, SectionWrapper.getFacial16Geoset(this));
    //setMeshEnabled(MeshIDType::Eyeglows, SectionWrapper.getFacial16Geoset(this));
}
