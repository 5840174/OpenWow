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

std::shared_ptr<CCharacter> CWorldObjectCreator::BuildCharacterFromTemplate(IRenderDevice& RenderDevice, IScene& Scene, const SCharacterTemplate& b, const std::shared_ptr<ISceneNode>& Parent)
{
	// 1. Load model
	auto characterM2Model = CreateCharacterModel(RenderDevice, b);
	if (characterM2Model == nullptr)
		return nullptr;

	std::shared_ptr<CCharacter> characterM2Instance = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<CCharacter>(characterM2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(characterM2Instance);

	// 2. Template
	characterM2Instance->Template() = b;

	// 3. Items
	characterM2Instance->RefreshCharacterItemsFromTemplate();

	// 4. Character textures
	characterM2Instance->Refresh_CreateSkinTexture(nullptr);

	// 5. Geosets
	characterM2Instance->RefreshMeshIDs();

	return characterM2Instance;
}

std::shared_ptr<CCharacter> CWorldObjectCreator::BuildCharacterFromDisplayInfo(IRenderDevice& RenderDevice, IScene& Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	if (rec == nullptr)
		throw CException("CWorldObjectCreator::BuildCharacterFromDisplayInfo: CreatureDisplayInfo don't contains id '%d'.", _id);

	uint32 humanoidData = rec->Get_HumanoidData();
	const DBC_CreatureDisplayInfoExtraRecord* humanoidRecExtra = m_DBCs->DBC_CreatureDisplayInfoExtra()[humanoidData];
	if (humanoidRecExtra == nullptr)
		throw CException("CWorldObjectCreator::BuildCharacterFromDisplayInfo: CreatureDisplayInfoExtra '%d' for CreatureDisplayInfo '%d'.", humanoidRecExtra, _id);

	// 1. Load model
	auto characterM2Model = CreateCreatureModel(RenderDevice, rec);
	if (characterM2Model == nullptr)
		return nullptr;

	std::shared_ptr<CCharacter> characterM2Instance = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<CCharacter>(characterM2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(characterM2Instance);

	// 2. Template
	{
		// 2.1 Visual params
		characterM2Instance->Template().Race = static_cast<Race>(m_DBCs->DBC_ChrRaces()[humanoidRecExtra->Get_Race()]->Get_ID());
		characterM2Instance->Template().Gender = static_cast<Gender>(humanoidRecExtra->Get_Gender());
		characterM2Instance->Template().skin = humanoidRecExtra->Get_SkinID();
		characterM2Instance->Template().face = humanoidRecExtra->Get_FaceID();
		characterM2Instance->Template().hairStyle = humanoidRecExtra->Get_HairStyleID();
		characterM2Instance->Template().hairColor = humanoidRecExtra->Get_HairColorID();
		characterM2Instance->Template().facialStyle = humanoidRecExtra->Get_FacialHairID();

		// 2.2 Items
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_HEAD] = SCharacterItemTemplate(humanoidRecExtra->Get_Helm(), DBCItem_EInventoryItemType::HEAD, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_SHOULDERS] = SCharacterItemTemplate(humanoidRecExtra->Get_Shoulder(), DBCItem_EInventoryItemType::SHOULDERS, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_BODY] = SCharacterItemTemplate(humanoidRecExtra->Get_Shirt(), DBCItem_EInventoryItemType::BODY, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_CHEST] = SCharacterItemTemplate(humanoidRecExtra->Get_Chest(), DBCItem_EInventoryItemType::CHEST, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_WAIST] = SCharacterItemTemplate(humanoidRecExtra->Get_Belt(), DBCItem_EInventoryItemType::WAIST, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_LEGS] = SCharacterItemTemplate(humanoidRecExtra->Get_Legs(), DBCItem_EInventoryItemType::LEGS, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_FEET] = SCharacterItemTemplate(humanoidRecExtra->Get_Boots(), DBCItem_EInventoryItemType::FEET, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_WRISTS] = SCharacterItemTemplate(humanoidRecExtra->Get_Wrist(), DBCItem_EInventoryItemType::WRISTS, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_HANDS] = SCharacterItemTemplate(humanoidRecExtra->Get_Gloves(), DBCItem_EInventoryItemType::HANDS, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_TABARD] = SCharacterItemTemplate(humanoidRecExtra->Get_Tabard(), DBCItem_EInventoryItemType::TABARD, 0);
		characterM2Instance->Template().ItemsTemplates[EQUIPMENT_SLOT_BACK] = SCharacterItemTemplate(humanoidRecExtra->Get_Cape(), DBCItem_EInventoryItemType::CLOAK, 0);
	}

	// 3. Items
	characterM2Instance->RefreshCharacterItemsFromTemplate();

	// 4. Creature textures
	characterM2Instance->Refresh_CreateSkinTexture(m_BaseManager.GetManager<IImagesFactory>()->CreateImage("Textures\\BakedNpcTextures\\" + humanoidRecExtra->Get_BakedSkin()));

	// 5. Geosets
	characterM2Instance->RefreshMeshIDs();

	return characterM2Instance;
}

std::shared_ptr<CCharacter> CWorldObjectCreator::BuildEmptyCharacterFromDisplayInfo(IRenderDevice & RenderDevice, IScene & Scene, uint32 _id, const std::shared_ptr<ISceneNode>& Parent)
{
	const DBC_CreatureDisplayInfoRecord* rec = m_DBCs->DBC_CreatureDisplayInfo()[_id];
	if (rec == nullptr)
		throw CException("CWorldObjectCreator::BuildCharacterFromDisplayInfo: CreatureDisplayInfo don't contains id '%d'.", _id);

	// 1. Load model
	auto characterM2Model = CreateCreatureModel(RenderDevice, rec);
	if (characterM2Model == nullptr)
		return nullptr;

	std::shared_ptr<CCharacter> characterM2Instance = ((Parent != nullptr) ? Parent : Scene.GetRootSceneNode())->CreateSceneNode<CCharacter>(characterM2Model);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(characterM2Instance);
	return characterM2Instance;
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

	std::shared_ptr<CM2> m2Object;
	try
	{
		m2Object = MakeShared(CM2, m_BaseManager, RenderDevice, Filename);
	}
	catch (const CException& e)
	{
		Log::Error("CWorldObjectCreator::LoadM2: Error '%s'.", e.MessageCStr());
		return nullptr;
	}

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

	auto wmoObject = MakeShared(CWMO, m_BaseManager, RenderDevice, Filename);

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

	//Log::Print("CWorldObjectCreator::LoadWMO: WMO '%s' loaded.", Filename.c_str());

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

std::shared_ptr<CM2> CWorldObjectCreator::CreateCharacterModel(IRenderDevice& RenderDevice, const SCharacterTemplate& CharacterTemplate)
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
