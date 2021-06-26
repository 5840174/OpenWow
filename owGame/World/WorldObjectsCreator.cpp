#include "stdafx.h"

// General
#include "WorldObjectsCreator.h"

CWorldObjectCreator::CWorldObjectCreator(IBaseManager & BaseManager)
	: m_BaseManager(BaseManager)
{
	m_DBCs = m_BaseManager.GetManager<CDBCStorage>();
}


#ifdef USE_M2_MODELS
//
// Factory
//
std::shared_ptr<Creature> CWorldObjectCreator::BuildCreatureFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	if (rec == nullptr)
		throw CException("CWorldObjectCreator::BuildCreatureFromDisplayInfo: CreatureDisplayInfo don't contains id '%d'.", _id);

	uint32 humanoidData = rec->Get_HumanoidData();
	const DBC_CreatureDisplayInfoExtraRecord* humanoidRecExtra = m_DBCs->DBC_CreatureDisplayInfoExtra()[humanoidData];
	if (humanoidRecExtra != nullptr)
		return BuildCharacterFromDisplayInfo(RenderDevice, Scene, _id, Parent);

	// 1. Load model
	auto m2Model = CreateCreatureModel(RenderDevice, rec);
	if (m2Model == nullptr)
		return nullptr;

	std::shared_ptr<Creature> newCreature = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<Creature>(m2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCreature);
	newCreature->setAlpha(static_cast<float>(rec->Get_Opacity()) / 255.0f);
	newCreature->SetScale(glm::vec3(rec->Get_Scale()));

	// 2. Creature textures
	{
		if (rec->Get_Texture1().length() != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_1, m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture1() + ".blp"));

		if (rec->Get_Texture2().length() != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_2, m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture2() + ".blp"));

		if (rec->Get_Texture3().length() != 0)
			newCreature->setSpecialTexture(SM2_Texture::Type::MONSTER_3, m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(m2Model->getFilePath() + rec->Get_Texture3() + ".blp"));
	}

	return newCreature;
}

std::shared_ptr<Character> CWorldObjectCreator::BuildCharacterFromTemplate(IRenderDevice& RenderDevice, IScene& Scene, const CInet_CharacterTemplate& b, const std::shared_ptr<ISceneNode>& Parent)
{
	// 1. Load model
	std::shared_ptr<CM2> m2Model = CreateCharacterModel(RenderDevice, b);
	if (m2Model == nullptr)
		return nullptr;

	Character_SectionWrapper sectionWrapper(m_BaseManager);

	std::shared_ptr<Character> newCharacter = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<Character>(m2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCharacter);

	// 2. Template
	{
		newCharacter->GetTemplate().TemplateSet(b);
	}

	// 3. Items
	{
		newCharacter->RefreshItemVisualData();
	}

	// 5. Character textures
	{
		Character_SkinTextureBaker skinTextureBaker(m_BaseManager, RenderDevice);
		auto skinTexture = skinTextureBaker.createTexture(newCharacter.get());
		newCharacter->RefreshTextures(sectionWrapper, skinTexture);
	}

	newCharacter->RefreshMeshIDs(sectionWrapper);

	return newCharacter;
}

std::shared_ptr<Character> CWorldObjectCreator::BuildCharacterFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	if (rec == nullptr)
		throw CException("CWorldObjectCreator::BuildCharacterFromDisplayInfo: CreatureDisplayInfo don't contains id '%d'.", _id);

	uint32 humanoidData = rec->Get_HumanoidData();
	const DBC_CreatureDisplayInfoExtraRecord* humanoidRecExtra = m_DBCs->DBC_CreatureDisplayInfoExtra()[humanoidData];
	if (humanoidRecExtra == nullptr)
		throw CException("CWorldObjectCreator::BuildCharacterFromDisplayInfo: CreatureDisplayInfoExtra '%d' for CreatureDisplayInfo '%d'.", humanoidRecExtra, _id);

	// 1. Load model
	auto m2Model = CreateCreatureModel(RenderDevice, rec);
	if (m2Model == nullptr)
		return nullptr;

	Character_SectionWrapper sectionWrapper(m_BaseManager);

	std::shared_ptr<Character> newCharacter = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<Character>(m2Model);
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
		if (false == bakedTextureName.empty())
		{
			bakedSkinTexture = m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D("Textures\\BakedNpcTextures\\" + bakedTextureName);
		}
		else
		{
			Log::Error("Character[%d]: Missing baked texture for humanoid[%d]. Create own. [%s]", rec->Get_ID(), humanoidRecExtra->Get_ID(), bakedTextureName.c_str());
		}
		newCharacter->RefreshTextures(sectionWrapper, bakedSkinTexture);
	}

	newCharacter->RefreshMeshIDs(sectionWrapper);

	return newCharacter;
}

std::shared_ptr<Character> CWorldObjectCreator::BuildEmptyCharacterFromDisplayInfo(IRenderDevice & RenderDevice, IScene & Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	if (rec == nullptr)
		throw CException("CWorldObjectCreator::BuildCharacterFromDisplayInfo: CreatureDisplayInfo don't contains id '%d'.", _id);

	// 1. Load model
	auto m2Model = CreateCreatureModel(RenderDevice, rec);
	if (m2Model == nullptr)
		return nullptr;

	std::shared_ptr<Character> newCharacter = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<Character>(m2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newCharacter);
	return newCharacter;
}

std::shared_ptr<ISceneNode> CWorldObjectCreator::BuildGameObjectFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	const DBC_GameObjectDisplayInfoRecord* gameObjectDisplayInfoRecord = m_DBCs->DBC_GameObjectDisplayInfo()[_id];
	if (gameObjectDisplayInfoRecord == nullptr)
		throw CException("CWorldObjectCreator::BuildGameObjectFromDisplayInfo: GameObjectDisplayInfo don't contains id '%d'.", _id);

	std::string modelName = gameObjectDisplayInfoRecord->Get_ModelName();
	modelName = Utils::ToLower(modelName);

	if (::strstr(modelName.c_str(), ".wmo") != NULL)
	{
		auto wmoModel = CreateGameObjectWMOModel(RenderDevice, gameObjectDisplayInfoRecord);

		std::shared_ptr<GameObjectWMO> newGameObject = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<GameObjectWMO>(wmoModel);
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newGameObject);

		return newGameObject;
	}
	else
	{
		auto m2Model = CreateGameObjectM2Model(RenderDevice, gameObjectDisplayInfoRecord);

		std::shared_ptr<GameObjectM2> newGameObject = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<GameObjectM2>(m2Model);
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(newGameObject);

		return newGameObject;
	}
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
	{
		std::lock_guard<std::mutex> lock(m_M2Lock);

		const auto& M2ObjectsWPtrsIt = m_M2ObjectsWPtrs.find(Filename);
		if (M2ObjectsWPtrsIt != m_M2ObjectsWPtrs.end())
		{
			if (M2ObjectsWPtrsIt->second.expired())
			{
				m_M2ObjectsWPtrs.erase(M2ObjectsWPtrsIt);
			}
			else if (auto M2ObjectsPtr = M2ObjectsWPtrsIt->second.lock())
			{
				return M2ObjectsPtr;
			}
			else
			{
				throw std::exception("Unknown issue.");
			}
		}
	}

	// Models blacklist
	//std::string newName = Utils::ToLower(Filename);
	//if (newName.find("orgrimmarsmokeemitter.mdx") != -1 || newName.find("orgrimmarfloatingembers.mdx") != -1)
	//	return nullptr;

	auto m2Object = std::make_shared<CM2>(m_BaseManager, RenderDevice, Filename);

	{
		std::lock_guard<std::mutex> lock(m_M2Lock);
		m_M2ObjectsWPtrs[Filename] = m2Object;
	}

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
	{
		std::lock_guard<std::mutex> lock(m_WMOLock);

		const auto& WMOObjectsWPtrsIt = m_WMOObjectsWPtrs.find(Filename);
		if (WMOObjectsWPtrsIt != m_WMOObjectsWPtrs.end())
		{
			if (WMOObjectsWPtrsIt->second.expired())
			{
				m_WMOObjectsWPtrs.erase(WMOObjectsWPtrsIt);
			}
			else if (auto WMOObjectsPtr = WMOObjectsWPtrsIt->second.lock())
			{
				return WMOObjectsPtr;
			}
			else
			{
				throw std::exception("Unknown issue.");
			}
		}
	}

	//std::string newName = Utils::ToLower(Filename);
	//if (newName.find(Utils::ToLower("WORLD\\WMO\\DUNGEON\\AZ_BLACKROCK\\BLACKROCK.WMO")) == std::string::npos)
	//	return nullptr;

	auto wmoObject = std::make_shared<CWMO>(m_BaseManager, RenderDevice, Filename);

	{
		std::lock_guard<std::mutex> lock(m_WMOLock);
		m_WMOObjectsWPtrs.insert(std::make_pair(Filename, wmoObject));
	}

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
	uint32 creatureModelDataID = CreatureDisplayInfo->Get_Model();
	const DBC_CreatureModelDataRecord* creatureModelDataRecord = m_DBCs->DBC_CreatureModelData()[creatureModelDataID];
	if (creatureModelDataRecord == nullptr)
		throw CException("CWorldObjectCreator::CreateCreatureModel: CreatureModelData don't contains id '%d'.", creatureModelDataID);

	std::string modelName = creatureModelDataRecord->Get_ModelPath();
	return LoadM2(RenderDevice, modelName);
}

std::shared_ptr<CM2> CWorldObjectCreator::CreateCharacterModel(IRenderDevice& RenderDevice, const CInet_CharacterTemplate& CharacterTemplate)
{
	std::string modelClientFileString = m_DBCs->DBC_ChrRaces()[(size_t)CharacterTemplate.Race]->Get_ClientFileString();
	std::string modelGender = (CharacterTemplate.Gender == Gender::Male) ? "Male" : "Female";
	std::string fullModelName = "Character\\" + modelClientFileString + "\\" + modelGender + "\\" + modelClientFileString + modelGender + ".M2";

	return LoadM2(RenderDevice, fullModelName);
}

std::shared_ptr<CM2> CWorldObjectCreator::CreateGameObjectM2Model(IRenderDevice & RenderDevice, const DBC_GameObjectDisplayInfoRecord * GameObjectDisplayInfoRecord)
{
	std::string modelName = GameObjectDisplayInfoRecord->Get_ModelName();
	return LoadM2(RenderDevice, modelName);
}
#endif



std::shared_ptr<CWMO> CWorldObjectCreator::CreateGameObjectWMOModel(IRenderDevice & RenderDevice, const DBC_GameObjectDisplayInfoRecord * GameObjectDisplayInfoRecord)
{
	std::string modelName = GameObjectDisplayInfoRecord->Get_ModelName();
	return LoadWMO(RenderDevice, modelName);
}

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
