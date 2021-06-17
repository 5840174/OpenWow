#include "stdafx.h"

// General
#include "WorldObjectsCreator.h"

CWorldObjectCreator::CWorldObjectCreator(IBaseManager & BaseManager)
	: m_BaseManager(BaseManager)
{
	m_DBCs = m_BaseManager.GetManager<CDBCStorage>();

	/*for (const auto& i : m_DBCs->DBC_CharacterFacialHairStyles())
	{
		Log::Info("%d %d %d %d %d %d %d %d %d", i->Get_Race(), i->Get_Gender(), i->Get_Variation(), i->Get_Group_01xx(), i->Get_Group_02xx(), i->Get_Group_03xx(), i->Get_Other0(), i->Get_Other1(), i->Get_Other2());
	}*/
}


#ifdef USE_M2_MODELS
//
// Factory
//
std::shared_ptr<Creature> CWorldObjectCreator::BuildCreatureFromDisplayInfo(IRenderDevice& RenderDevice, IScene* Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	if (rec == nullptr)
		return nullptr;

	const DBC_CreatureDisplayInfoExtraRecord* humanoidRecExtra = m_DBCs->DBC_CreatureDisplayInfoExtra()[rec->Get_HumanoidData()];
	if (humanoidRecExtra != 0)
		return BuildCharactedFromDisplayInfo(RenderDevice, Scene, _id, Parent);

	// 1. Load model
	std::shared_ptr<CM2> m2Model = CreateCreatureModel(RenderDevice, rec);
	if (m2Model == nullptr)
		return nullptr;

	std::shared_ptr<Creature> newCreature = Scene->CreateSceneNode<Creature>(Parent, m2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCreature);
	newCreature->setAlpha(static_cast<float>(rec->Get_Opacity()) / 255.0f);
	newCreature->SetScale(glm::vec3(rec->Get_Scale()));

	// 2. Creature textures
	{
		if (rec->Get_Texture1().length() != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_1, RenderDevice.GetObjectsFactory().LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture1() + ".blp"));

		if (rec->Get_Texture2().length() != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_2, RenderDevice.GetObjectsFactory().LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture2() + ".blp"));

		if (rec->Get_Texture3().length() != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_3, RenderDevice.GetObjectsFactory().LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture3() + ".blp"));
	}

	return newCreature;
}

std::shared_ptr<Character> CWorldObjectCreator::BuildCharactedFromTemplate(IRenderDevice& RenderDevice, IScene* Scene, const CInet_CharacterTemplate& b, const std::shared_ptr<ISceneNode>& Parent)
{
	// 1. Load model
	std::shared_ptr<CM2> m2Model = CreateCharacterModel(RenderDevice, b);
	if (m2Model == nullptr)
		return nullptr;

	std::shared_ptr<Character> newCharacter = Scene->CreateSceneNode<Character>(Parent, m2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCharacter);

	// 2. Template
	{
		newCharacter->GetTemplate().TemplateSet(b);
	}

	// 3 Refresh
	Character_SectionWrapper sectionWrapper(m_BaseManager, RenderDevice);

	newCharacter->RefreshItemVisualData();

	Character_SkinTextureBaker skinTextureBaker(m_BaseManager, RenderDevice);
	auto skinTexture = skinTextureBaker.createTexture(newCharacter.get());
	newCharacter->RefreshTextures(sectionWrapper, skinTexture);

	newCharacter->RefreshMeshIDs(sectionWrapper);

	return newCharacter;
}

std::shared_ptr<Character> CWorldObjectCreator::BuildCharactedFromDisplayInfo(IRenderDevice& RenderDevice, IScene * Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	_ASSERT(rec != nullptr);

	const DBC_CreatureDisplayInfoExtraRecord* humanoidRecExtra = m_DBCs->DBC_CreatureDisplayInfoExtra()[rec->Get_HumanoidData()];
	_ASSERT(humanoidRecExtra != nullptr);

	// 1. Load model
	std::shared_ptr<CM2> m2Model = CreateCreatureModel(RenderDevice, rec);
	if (m2Model == nullptr)
		return nullptr;

	std::shared_ptr<Character> newCharacter = Scene->CreateSceneNode<Character>(Parent, m2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCharacter);

	// 2. Template
	{
		CInet_CharacterTemplate characterTemplate;

		// 2.1 Visual params
		characterTemplate.Race = (Race)m_DBCs->DBC_ChrRaces()[humanoidRecExtra->Get_Race()]->Get_ID();
		characterTemplate.Gender = (Gender)humanoidRecExtra->Get_Gender();
		characterTemplate.skin = humanoidRecExtra->Get_SkinID();
		characterTemplate.face = humanoidRecExtra->Get_FaceID();
		characterTemplate.hairStyle = humanoidRecExtra->Get_HairStyleID();
		characterTemplate.hairColor = humanoidRecExtra->Get_HairColorID();
		characterTemplate.facialStyle = humanoidRecExtra->Get_FacialHairID();

		// 2.2 Items
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_HEAD] = CInet_ItemTemplate(humanoidRecExtra->Get_Helm(), EInventoryType::HEAD, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_SHOULDERS] = CInet_ItemTemplate(humanoidRecExtra->Get_Shoulder(), EInventoryType::SHOULDERS, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_BODY] = CInet_ItemTemplate(humanoidRecExtra->Get_Shirt(), EInventoryType::BODY, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_CHEST] = CInet_ItemTemplate(humanoidRecExtra->Get_Chest(), EInventoryType::CHEST, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_WAIST] = CInet_ItemTemplate(humanoidRecExtra->Get_Belt(), EInventoryType::WAIST, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_LEGS] = CInet_ItemTemplate(humanoidRecExtra->Get_Legs(), EInventoryType::LEGS, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_FEET] = CInet_ItemTemplate(humanoidRecExtra->Get_Boots(), EInventoryType::FEET, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_WRISTS] = CInet_ItemTemplate(humanoidRecExtra->Get_Wrist(), EInventoryType::WRISTS, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_HANDS] = CInet_ItemTemplate(humanoidRecExtra->Get_Gloves(), EInventoryType::HANDS, 0);
		characterTemplate.ItemsTemplates[EQUIPMENT_SLOT_TABARD] = CInet_ItemTemplate(humanoidRecExtra->Get_Tabard(), EInventoryType::TABARD, 0);
		//ItemsTemplates[EQUIPMENT_SLOT_BACK] = CInet_ItemTemplate(humanoidRecExtra->Get_Cape(), EInventoryType::CLOAK, 0);

		newCharacter->GetTemplate().TemplateSet(characterTemplate);
	}

	// 3. Items
	{
		newCharacter->RefreshItemVisualData();
	}

	// 4. Creature textures
	{
		std::string bakedTextureName = humanoidRecExtra->Get_BakedSkin();
		std::shared_ptr<ITexture> bakedSkinTexture = nullptr;
		if (!bakedTextureName.empty())
		{
			bakedSkinTexture = RenderDevice.GetObjectsFactory().LoadTexture2D("Textures\\BakedNpcTextures\\" + bakedTextureName);
		}
		else
		{
			Log::Error("Character[%d]: Missing baked texture for humanoid[%d]. Create own. [%s]", rec->Get_ID(), humanoidRecExtra->Get_ID(), bakedTextureName.c_str());
		}

		Character_SectionWrapper sectionWrapper(m_BaseManager, RenderDevice);

		newCharacter->RefreshTextures(sectionWrapper, bakedSkinTexture);
	}

	Character_SectionWrapper sectionWrapper(m_BaseManager, RenderDevice);
	newCharacter->RefreshMeshIDs(sectionWrapper);

	return newCharacter;
}

std::shared_ptr<GameObject> CWorldObjectCreator::BuildGameObjectFromDisplayInfo(IRenderDevice & RenderDevice, IScene * Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	std::shared_ptr<CM2> m2Model = nullptr;
	{
		m2Model = CreateGameObjectModel(RenderDevice, m_DBCs->DBC_GameObjectDisplayInfo()[_id]);
		if (m2Model == nullptr)
			return nullptr;
	}

	std::shared_ptr<GameObject> newGameObject = Scene->CreateSceneNode<GameObject>(Parent, m2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newGameObject);
	return newGameObject;
}
#endif


//
// IWoWObjectsCreator
//
void CWorldObjectCreator::ClearCache()
{
}

#ifdef USE_M2_MODELS
std::shared_ptr<CM2> CWorldObjectCreator::LoadM2(IRenderDevice& RenderDevice, const std::string& Filename, bool ImmediateLoad)
{
	std::lock_guard<std::mutex> lock(m_M2Lock);

	const auto& M2ObjectsWPtrsIt = m_M2ObjectsWPtrs.find(Filename);
	
	// Already exists
	if (M2ObjectsWPtrsIt != m_M2ObjectsWPtrs.end())
	{
		if (auto M2ObjectsPtr = M2ObjectsWPtrsIt->second.lock())
		{
			return M2ObjectsPtr;
		}
		else
		{
			m_M2ObjectsWPtrs.erase(M2ObjectsWPtrsIt);
		}
	}

	// Models blacklist
	std::string newName = Utils::ToLower(Filename);
	if (newName.find("orgrimmarsmokeemitter.mdx") != -1 || newName.find("orgrimmarfloatingembers.mdx") != -1)
		return nullptr;

	std::shared_ptr<CM2> m2Object;
	try
	{
		m2Object = std::make_shared<CM2>(m_BaseManager, RenderDevice, Filename);
	}
	catch (const CException& e)
	{
		Log::Error("CWorldObjectCreator: Error while creating M2 model: '%s'.", e.Message().c_str());
		return nullptr;
	}

	m_M2ObjectsWPtrs[Filename] = m2Object;

	if (ImmediateLoad)
	{
		m2Object->Load();
		m2Object->SetState(ILoadable::ELoadableState::Loaded);
	}
	else
	{
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(m2Object);
	}

	return m2Object;
}
#endif

std::shared_ptr<CWMO> CWorldObjectCreator::LoadWMO(IRenderDevice& RenderDevice, const std::string& Filename, bool ImmediateLoad)
{
	std::lock_guard<std::mutex> lock(m_WMOLock);

	const auto& WMOObjectsWPtrsIt = m_WMOObjectsWPtrs.find(Filename);

	// Already exists
	if (WMOObjectsWPtrsIt != m_WMOObjectsWPtrs.end())
	{
		if (auto WMOObjectsPtr = WMOObjectsWPtrsIt->second.lock())
		{
			return WMOObjectsPtr;
		}
		else
		{
			m_WMOObjectsWPtrs.erase(WMOObjectsWPtrsIt);
		}
	}

	std::shared_ptr<CWMO> wmoObject = std::make_shared<CWMO>(m_BaseManager, RenderDevice, Filename);
	m_WMOObjectsWPtrs.insert(std::make_pair(Filename, wmoObject));
	
	if (ImmediateLoad)
	{
		wmoObject->Load();
		wmoObject->SetState(ILoadable::ELoadableState::Loaded);
	}
	else
	{
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(wmoObject);
	}

	return wmoObject;
}

void CWorldObjectCreator::InitEGxBlend(IRenderDevice& RenderDevice)
{
	for (uint32 i = 0; i < 14; i++)
	{
		if (i == 11)
			continue;

		std::shared_ptr<IBlendState> blendState = RenderDevice.GetObjectsFactory().CreateBlendState();
		blendState->SetBlendMode(GetEGxBlendMode(i));
		m_EGxBlendStates[i] = blendState;
	}
}

std::shared_ptr<IBlendState> CWorldObjectCreator::GetEGxBlend(uint32 Index) const
{
	return m_EGxBlendStates.at(Index);
}



//
// Private 
//
#ifdef USE_M2_MODELS

std::shared_ptr<CM2> CWorldObjectCreator::CreateCreatureModel(IRenderDevice& RenderDevice, const DBC_CreatureDisplayInfoRecord* CreatureDisplayInfo)
{
	const DBC_CreatureModelDataRecord* modelRec = m_DBCs->DBC_CreatureModelData()[CreatureDisplayInfo->Get_Model()];
	_ASSERT(modelRec != nullptr);

	std::string modelName = modelRec->Get_ModelPath();
	return LoadM2(RenderDevice, modelName);
}

std::shared_ptr<CM2> CWorldObjectCreator::CreateCharacterModel(IRenderDevice& RenderDevice, const CInet_CharacterTemplate& CharacterTemplate)
{
	std::string modelClientFileString = m_DBCs->DBC_ChrRaces()[(size_t)CharacterTemplate.Race]->Get_ClientFileString();
	std::string modelGender = (CharacterTemplate.Gender == Gender::Male) ? "Male" : "Female";
	std::string fullModelName = "Character\\" + modelClientFileString + "\\" + modelGender + "\\" + modelClientFileString + modelGender + ".M2";

	return LoadM2(RenderDevice, fullModelName);
}

std::shared_ptr<CM2> CWorldObjectCreator::CreateGameObjectModel(IRenderDevice & RenderDevice, const DBC_GameObjectDisplayInfoRecord * GameObjectDisplayInfoRecord)
{
	if (GameObjectDisplayInfoRecord == nullptr)
		return nullptr;

	std::string modelName = GameObjectDisplayInfoRecord->Get_ModelName();
	return LoadM2(RenderDevice, modelName);
}
#endif



IBlendState::BlendMode CWorldObjectCreator::GetEGxBlendMode(uint32 Index)
{
	switch (Index)
	{
	case 0: // Opaque
		return IBlendState::BlendMode(false, false,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
		break;

	case 1: // AlphaKey
		return IBlendState::BlendMode(false, false,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
		break;

	case 2: // Alpha
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::OneMinusSrcAlpha,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha);
		break;

	case 3: // Add
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::Zero, IBlendState::BlendFactor::One);
		break;

	case 4: // Mod
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::Zero,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::Zero);
		break;

	case 5: // Mod2x
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::SrcColor,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::SrcAlpha);
		break;

	case 6: // ModAdd
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::DstColor, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::DstAlpha, IBlendState::BlendFactor::One);
		break;

	case 7: // InvSrcAlphaAdd
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::One);
		break;

	case 8: // InvSrcAlphaOpaque
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::Zero,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::OneMinusSrcAlpha, IBlendState::BlendFactor::Zero);
		break;

	case 9: // SrcAlphaOpaque
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::Zero,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::SrcAlpha, IBlendState::BlendFactor::Zero);
		break;

	case 10: // NoAlphaAdd
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::Zero, IBlendState::BlendFactor::One);

	case 11: // ConstantAlpha
		//(true, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
		_ASSERT_EXPR(false, "Constant alpha EGxBlend doesn't support");
		break;

	case 12: // Screen
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::OneMinusDstColor, IBlendState::BlendFactor::One,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::Zero);
		break;

	case 13: // BlendAdd
		return IBlendState::BlendMode(true, false,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha,
			IBlendState::BlendOperation::Add,
			IBlendState::BlendFactor::One, IBlendState::BlendFactor::OneMinusSrcAlpha);
		break;

	default:
		_ASSERT(false);
	}

	_ASSERT(false);
	return IBlendState::BlendMode();
}
