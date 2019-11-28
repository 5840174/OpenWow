#include "stdafx.h"

// General
#include "Character.h"

// Additional
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

Character::Character() 
	: Creature()
{


	setMeshEnabled(MeshIDType::Ears, EarsStyles::Enabled);
	setMeshEnabled(MeshIDType::Eyeglows, EyeglowsStyles::Racial);
}

Character::~Character()
{
}

void Character::InitFromTemplate(const CharacterTemplate& b)
{
	// TODO: Move me outside construtor
	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++)
	{
		m_VisualItems.push_back(std::make_shared<CItem_VisualData>(std::static_pointer_cast<Character, SceneNode3D>(shared_from_this())));
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

void Character::InitFromDisplayInfo(uint32 _id)
{
	// TODO: Move me outside construtor
	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++)
	{
		m_VisualItems.push_back(std::make_shared<CItem_VisualData>(std::static_pointer_cast<Character, SceneNode3D>(shared_from_this())));
	}
	// TODO: Move me outside construtor


	std::shared_ptr<const DBC_CreatureDisplayInfoRecord> rec = DBC_CreatureDisplayInfo[_id];
	_ASSERT(rec != nullptr);

	std::shared_ptr<const DBC_CreatureDisplayInfoExtraRecord> humanoidRecExtra = rec->Get_HumanoidData();
	_ASSERT(humanoidRecExtra != nullptr);

	// 1. Template
	{
		// 1.1 Visual params
        m_Template.Race = (Race::List)humanoidRecExtra->Get_Race()->Get_ID();
        m_Template.Gender = (Gender::List)humanoidRecExtra->Get_Gender();
        m_Template.skin = humanoidRecExtra->Get_SkinColor();
        m_Template.face = humanoidRecExtra->Get_FaceType();
        m_Template.hairStyle = humanoidRecExtra->Get_HairType();
        m_Template.hairColor = humanoidRecExtra->Get_HairStyleOrColor();
        m_Template.facialStyle = humanoidRecExtra->Get_BeardStyle();

		// 1.2 Items
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_HEAD] = ItemTemplate(humanoidRecExtra->Get_Helm(), InventoryType::HEAD, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_SHOULDERS] = ItemTemplate(humanoidRecExtra->Get_Shoulder(), InventoryType::SHOULDERS, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_BODY] = ItemTemplate(humanoidRecExtra->Get_Shirt(), InventoryType::BODY, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_CHEST] = ItemTemplate(humanoidRecExtra->Get_Chest(), InventoryType::CHEST, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_WAIST] = ItemTemplate(humanoidRecExtra->Get_Belt(), InventoryType::WAIST, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_LEGS] = ItemTemplate(humanoidRecExtra->Get_Legs(), InventoryType::LEGS, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_FEET] = ItemTemplate(humanoidRecExtra->Get_Boots(), InventoryType::FEET, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_WRISTS] = ItemTemplate(humanoidRecExtra->Get_Wrist(), InventoryType::WRISTS, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_HANDS] = ItemTemplate(humanoidRecExtra->Get_Gloves(), InventoryType::HANDS, 0);
        m_Template.ItemsTemplates[EQUIPMENT_SLOT_TABARD] = ItemTemplate(humanoidRecExtra->Get_Tabard(), InventoryType::TABARD, 0);
		//ItemsTemplates[EQUIPMENT_SLOT_BACK] = ItemTemplate(humanoidRecExtra->Get_Cape(), InventoryType::CLOAK, 0);
	}

	// 2. Load model
	{
		// We ???always??? can load model from (CreatureDisplayInfo->Model)
		CreateCreatureModel(rec);
	}

	// 3. Items
	{
		RefreshItemVisualData();
	}

	// 4. Creature textures
	{
		std::string bakedTextureName = humanoidRecExtra->Get_BakedSkin();
		std::shared_ptr<Texture> bakedSkinTexture = nullptr;
		if (!bakedTextureName.empty())
		{
			bakedSkinTexture = _RenderDevice->CreateTexture2D("Textures\\BakedNpcTextures\\" + bakedTextureName);
		}
		else
		{
			Log::Error("Character[%d]: Missing baked texture for humanoid[%d]. Create own.", rec->Get_ID(), humanoidRecExtra->Get_ID());
		}
	
		RefreshTextures(bakedSkinTexture);
	}

	RefreshMeshIDs();
}

// Specific for character creation
void Character::InitFromDisplayInfoCreating(uint32 _id, Race::List _race, Gender::List _gender)
{
	// TODO: Move me outside construtor
	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++)
	{
		m_VisualItems.push_back(std::make_shared<CItem_VisualData>(std::static_pointer_cast<Character, SceneNode3D>(shared_from_this())));
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

//--

void Character::CreateCharacterModel()
{
	std::string modelClientFileString = DBC_ChrRaces[m_Template.Race]->Get_ClientFileString();
	std::string modelGender = (m_Template.Gender == Gender::Male) ? "Male" : "Female";
	std::string fullModelName = "Character\\" + modelClientFileString + "\\" + modelGender + "\\" + modelClientFileString + modelGender + ".M2";

	std::shared_ptr<M2> model = GetManager<IM2Manager>(_ApplicationInstance->GetBaseManager())->Add(fullModelName);
	_ASSERT(model != nullptr);

	setM2(model);
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

void Character::RefreshTextures(std::shared_ptr<Texture> _skin)
{
	if (_skin == nullptr)
	{
		Character_SkinTextureBaker baker;
		_skin = baker.createTexture(this);
	}

	setSpecialTexture(SM2_Texture::Type::SKIN, _skin);
	setSpecialTexture(SM2_Texture::Type::SKIN_EXTRA, Character_SectionWrapper::getSkinExtraTexture(this));
	setSpecialTexture(SM2_Texture::Type::CHAR_HAIR, Character_SectionWrapper::getHairTexture(this));

	// Cloak
	std::shared_ptr<const CItem_VisualData> item = m_VisualItems[EQUIPMENT_SLOT_BACK];
	if (item->InventoryType != InventoryType::NON_EQUIP)
	{
		_ASSERT(item->getObjectComponents().size() == 1);
		std::shared_ptr<Texture> cloackTexttre = item->getObjectComponents()[0].texture;
		setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, cloackTexttre);
	}
}

void Character::RefreshMeshIDs()
{
	setHairGeoset   (Character_SectionWrapper::getHairGeoset(this));
	setFacial1Geoset(Character_SectionWrapper::getFacial1Geoset(this));
	setFacial2Geoset(Character_SectionWrapper::getFacial2Geoset(this));
	setFacial3Geoset(Character_SectionWrapper::getFacial3Geoset(this));

    setMeshEnabled(MeshIDType::Unk2, Character_SectionWrapper::getFacial16Geoset(this));
    setMeshEnabled(MeshIDType::Eyeglows, Character_SectionWrapper::getFacial16Geoset(this));
}
