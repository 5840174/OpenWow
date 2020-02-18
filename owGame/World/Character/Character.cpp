#include "stdafx.h"

// General
#include "Character.h"

// Additional
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

Character::Character(const M2& M2Object)
	: Creature(M2Object)
{
	setMeshEnabled(MeshIDType::Ears, EarsStyles::Enabled);
	setMeshEnabled(MeshIDType::Eyeglows, EyeglowsStyles::Racial);
}

Character::~Character()
{
}

/*
void Character::InitFromTemplate(const CharacterTemplate& b)
{
	// TODO: Move me outside construtor
	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++)
	{
		m_VisualItems.push_back(std::make_shared<CItem_VisualData>(std::static_pointer_cast<Character>(shared_from_this())));
	}
	// TODO: Move me outside construtor

	// 1. Template
	{
        m_Template.TemplateSet(b);
	}

	// 2. Load model
	{
		CreateCharacterModel();
	}

	// 3 Refresh
	RefreshItemVisualData();
	RefreshTextures();
	RefreshMeshIDs();
}
*/

// Specific for character creation
#if 0
void Character::InitFromDisplayInfoCreating(uint32 _id, Race::List _race, Gender::List _gender)
{
	// TODO: Move me outside construtor
	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++)
	{
		m_VisualItems.push_back(std::make_shared<CItem_VisualData>(std::static_pointer_cast<Character>(shared_from_this())));
	}
	// TODO: Move me outside construtor


	std::shared_ptr<const DBC_CreatureDisplayInfoRecord> rec = DBC_CreatureDisplayInfo[_id];
	_ASSERT(rec != nullptr);

	std::shared_ptr<const DBC_CreatureDisplayInfoExtraRecord> humanoidRecExtra = rec->Get_HumanoidData();
	_ASSERT(humanoidRecExtra == nullptr);

	// 1. Template
	{
		// 1.1 Visual params
        m_Template.Race = _race;
        m_Template.Gender = _gender;

		// 1.2 Items
		/*ItemsTemplates[EQUIPMENT_SLOT_HEAD] = ItemTemplate(humanoidRecExtra->Get_Helm(), InventoryType::HEAD, 0);
		ItemsTemplates[EQUIPMENT_SLOT_SHOULDERS] = ItemTemplate(humanoidRecExtra->Get_Shoulder(), InventoryType::SHOULDERS, 0);
		ItemsTemplates[EQUIPMENT_SLOT_BODY] = ItemTemplate(humanoidRecExtra->Get_Shirt(), InventoryType::BODY, 0);
		ItemsTemplates[EQUIPMENT_SLOT_CHEST] = ItemTemplate(humanoidRecExtra->Get_Chest(), InventoryType::CHEST, 0);
		ItemsTemplates[EQUIPMENT_SLOT_WAIST] = ItemTemplate(humanoidRecExtra->Get_Belt(), InventoryType::WAIST, 0);
		ItemsTemplates[EQUIPMENT_SLOT_LEGS] = ItemTemplate(humanoidRecExtra->Get_Legs(), InventoryType::LEGS, 0);
		ItemsTemplates[EQUIPMENT_SLOT_FEET] = ItemTemplate(humanoidRecExtra->Get_Boots(), InventoryType::FEET, 0);
		ItemsTemplates[EQUIPMENT_SLOT_WRISTS] = ItemTemplate(humanoidRecExtra->Get_Wrist(), InventoryType::WRISTS, 0);
		ItemsTemplates[EQUIPMENT_SLOT_HANDS] = ItemTemplate(humanoidRecExtra->Get_Gloves(), InventoryType::HANDS, 0);
		ItemsTemplates[EQUIPMENT_SLOT_TABARD] = ItemTemplate(humanoidRecExtra->Get_Tabard(), InventoryType::TABARD, 0);
		ItemsTemplates[EQUIPMENT_SLOT_BACK] = ItemTemplate(humanoidRecExtra->Get_Cape(), InventoryType::CLOAK, 0);*/
	}

	// 2. Load model
	{
		CreateCreatureModel(rec);
	}

	// 3 Refresh
	RefreshItemVisualData();
	RefreshTextures();
	RefreshMeshIDs();
}
#endif

//--

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

void Character::RefreshTextures(const Character_SectionWrapper& SectionWrapper, const Character_SkinTextureBaker& SkinTextureBaker, std::shared_ptr<ITexture>& _skin)
{
	if (_skin == nullptr)
		_skin = SkinTextureBaker.createTexture(this);

	setSpecialTexture(SM2_Texture::Type::SKIN, _skin);
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
	setMeshEnabled(MeshIDType::SkinAndHair, SectionWrapper.getHairGeoset(this));
	setMeshEnabled(MeshIDType::Facial1, SectionWrapper.getFacial1Geoset(this));
	setMeshEnabled(MeshIDType::Facial2, SectionWrapper.getFacial2Geoset(this));
	setMeshEnabled(MeshIDType::Facial3, SectionWrapper.getFacial3Geoset(this));

    setMeshEnabled(MeshIDType::Unk2, SectionWrapper.getFacial16Geoset(this));
    setMeshEnabled(MeshIDType::Eyeglows, SectionWrapper.getFacial16Geoset(this));
}
