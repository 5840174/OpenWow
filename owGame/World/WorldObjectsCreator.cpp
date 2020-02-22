#include "stdafx.h"

// General
#include "WorldObjectsCreator.h"

CWorldObjectCreator::CWorldObjectCreator(IBaseManager & BaseManager, IRenderDevice & RenderDevice)
	: m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
{
	m_DBCs = m_BaseManager.GetManager<CDBCStorage>();
	m_M2Manager = m_BaseManager.GetManager<IM2Manager>();


	/*for (const auto& i : m_DBCs->DBC_CharacterFacialHairStyles())
	{
		Log::Info("%d %d %d %d %d %d %d %d %d", i->Get_Race(), i->Get_Gender(), i->Get_Variation(), i->Get_Group_01xx(), i->Get_Group_02xx(), i->Get_Group_03xx(), i->Get_Other0(), i->Get_Other1(), i->Get_Other2());
	}*/
}



//
// Factory
//
std::shared_ptr<Creature> CWorldObjectCreator::BuildCreatureFromDisplayInfo(IScene* Scene, uint32 _id, const std::shared_ptr<ISceneNode3D>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	if (rec == nullptr)
		throw std::exception("Not found!");

	const DBC_CreatureDisplayInfoExtraRecord* humanoidRecExtra = m_DBCs->DBC_CreatureDisplayInfoExtra()[rec->Get_HumanoidData()];
	if (humanoidRecExtra != 0)
		return BuildCharactedFromDisplayInfo(Scene, _id, nullptr);

	// 1. Load model
	std::shared_ptr<M2> m2Model = nullptr;
	{
		m2Model = CreateCreatureModel(rec);
	}

	std::shared_ptr<Creature> newCreature = Scene->CreateSceneNode<Creature>(Parent, *m2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCreature);
	newCreature->setAlpha(static_cast<float>(rec->Get_Opacity()) / 255.0f);
	//newCreature->SetScale(rec->Get_Scale());

	// 2. Creature textures
	{
		if (strlen(rec->Get_Texture1()) != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_1, m_RenderDevice.GetObjectsFactory().LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture1() + ".blp"));

		if (strlen(rec->Get_Texture2()) != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_2, m_RenderDevice.GetObjectsFactory().LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture2() + ".blp"));

		if (strlen(rec->Get_Texture3()) != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_3, m_RenderDevice.GetObjectsFactory().LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture3() + ".blp"));
	}

	return newCreature;
}

std::shared_ptr<Character> CWorldObjectCreator::BuildCharactedFromTemplate(IScene* Scene, const CInet_CharacterTemplate& b, const std::shared_ptr<ISceneNode3D>& Parent)
{
	// 1. Load model
	std::shared_ptr<M2> m2Model = nullptr;
	{
		m2Model = CreateCharacterModel(b);
	}

	std::shared_ptr<Character> newCharacter = Scene->CreateSceneNode<Character>(Parent, *m2Model);
	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++) // TODO: Move to constuctor
		newCharacter->GetVisualItems().push_back(std::make_shared<CItem_VisualData>(m_BaseManager, m_RenderDevice, *newCharacter));
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCharacter);

	// 2. Template
	{
		newCharacter->GetTemplate().TemplateSet(b);
	}

	// 3 Refresh
	Character_SectionWrapper sectionWrapper(m_BaseManager, m_RenderDevice);

	newCharacter->RefreshItemVisualData();

	Character_SkinTextureBaker skinTextureBaker(m_BaseManager, m_RenderDevice);
	auto skinTexture = skinTextureBaker.createTexture(newCharacter.get());
	newCharacter->RefreshTextures(sectionWrapper, skinTexture);

	newCharacter->RefreshMeshIDs(sectionWrapper);

	return newCharacter;
}

std::shared_ptr<Character> CWorldObjectCreator::BuildCharactedFromDisplayInfo(IScene * Scene, uint32 _id, const std::shared_ptr<ISceneNode3D>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	_ASSERT(rec != nullptr);

	const DBC_CreatureDisplayInfoExtraRecord* humanoidRecExtra = m_DBCs->DBC_CreatureDisplayInfoExtra()[rec->Get_HumanoidData()];
	_ASSERT(humanoidRecExtra != nullptr);

	// 1. Load model
	std::shared_ptr<M2> m2Model = nullptr;
	{
		// We ???always??? can load model from (CreatureDisplayInfo->Model)
		m2Model = CreateCreatureModel(rec);
	}

	std::shared_ptr<Character> newCharacter = Scene->CreateSceneNode<Character>(Parent, *m2Model);
	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++) // TODO: Move to constuctor
		newCharacter->GetVisualItems().push_back(std::make_shared<CItem_VisualData>(m_BaseManager, m_RenderDevice, *newCharacter));
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCharacter);

	// 2. Template
	{
		CInet_CharacterTemplate characterTemplate;

		// 2.1 Visual params
		characterTemplate.Race = (Race::List)m_DBCs->DBC_ChrRaces()[humanoidRecExtra->Get_Race()]->Get_ID();
		characterTemplate.Gender = (Gender::List)humanoidRecExtra->Get_Gender();
		characterTemplate.skin = humanoidRecExtra->Get_SkinID();
		characterTemplate.face = humanoidRecExtra->Get_FaceID();
		characterTemplate.hairStyle = humanoidRecExtra->Get_HairStyleID();
		characterTemplate.hairColor = humanoidRecExtra->Get_HairColorID();
		characterTemplate.facialStyle = humanoidRecExtra->Get_FacialHairID();

		// 2.2 Items
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_HEAD] = CInet_ItemTemplate(humanoidRecExtra->Get_Helm(), InventoryType::HEAD, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_SHOULDERS] = CInet_ItemTemplate(humanoidRecExtra->Get_Shoulder(), InventoryType::SHOULDERS, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_BODY] = CInet_ItemTemplate(humanoidRecExtra->Get_Shirt(), InventoryType::BODY, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_CHEST] = CInet_ItemTemplate(humanoidRecExtra->Get_Chest(), InventoryType::CHEST, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_WAIST] = CInet_ItemTemplate(humanoidRecExtra->Get_Belt(), InventoryType::WAIST, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_LEGS] = CInet_ItemTemplate(humanoidRecExtra->Get_Legs(), InventoryType::LEGS, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_FEET] = CInet_ItemTemplate(humanoidRecExtra->Get_Boots(), InventoryType::FEET, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_WRISTS] = CInet_ItemTemplate(humanoidRecExtra->Get_Wrist(), InventoryType::WRISTS, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_HANDS] = CInet_ItemTemplate(humanoidRecExtra->Get_Gloves(), InventoryType::HANDS, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_TABARD] = CInet_ItemTemplate(humanoidRecExtra->Get_Tabard(), InventoryType::TABARD, 0);
		//ItemsTemplates[EQUIPMENT_SLOT_BACK] = CInet_ItemTemplate(humanoidRecExtra->Get_Cape(), InventoryType::CLOAK, 0);

		newCharacter->GetTemplate().TemplateSet(characterTemplate);
	}

	// 3. Items
	{
		//newCharacter->RefreshItemVisualData();
	}

	// 4. Creature textures
	{
		std::string bakedTextureName = humanoidRecExtra->Get_BakedSkin();
		std::shared_ptr<ITexture> bakedSkinTexture = nullptr;
		if (!bakedTextureName.empty())
		{
			bakedSkinTexture = m_RenderDevice.GetObjectsFactory().LoadTexture2D("Textures\\BakedNpcTextures\\" + bakedTextureName);
		}
		else
		{
			Log::Error("Character[%d]: Missing baked texture for humanoid[%d]. Create own.", rec->Get_ID(), humanoidRecExtra->Get_ID());
		}

		Character_SectionWrapper sectionWrapper(m_BaseManager, m_RenderDevice);

		newCharacter->RefreshTextures(sectionWrapper, bakedSkinTexture);
	}

	Character_SectionWrapper sectionWrapper(m_BaseManager, m_RenderDevice);
	newCharacter->RefreshMeshIDs(sectionWrapper);

	return newCharacter;
}



//
// Private 
//
std::shared_ptr<M2> CWorldObjectCreator::CreateCreatureModel(const DBC_CreatureDisplayInfoRecord* CreatureDisplayInfo)
{
	const DBC_CreatureModelDataRecord* modelRec = m_DBCs->DBC_CreatureModelData()[CreatureDisplayInfo->Get_Model()];
	_ASSERT(modelRec != nullptr);

	std::string modelName = modelRec->Get_ModelPath();
	return m_M2Manager->Add(m_RenderDevice, modelName);
}

std::shared_ptr<M2> CWorldObjectCreator::CreateCharacterModel(const CInet_CharacterTemplate& CharacterTemplate)
{
	std::string modelClientFileString = m_DBCs->DBC_ChrRaces()[CharacterTemplate.Race]->Get_ClientFileString();
	std::string modelGender = (CharacterTemplate.Gender == Gender::Male) ? "Male" : "Female";
	std::string fullModelName = "Character\\" + modelClientFileString + "\\" + modelGender + "\\" + modelClientFileString + modelGender + ".M2";

	return m_M2Manager->Add(m_RenderDevice, fullModelName);
}