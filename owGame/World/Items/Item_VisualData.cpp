#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "World/Character/Character.h"

// General
#include "Item_VisualData.h"

#define MESHID_MAX_MODS 3
#define MESHID_ALLUNK { MeshIDType::UNK, MeshIDType::UNK, MeshIDType::UNK }
#define ATTACHS_MAX 2

// Object components
struct
{
	EInventoryType	            slot;
	const char* const			folder = "";
	MeshIDType					modifiers[MESHID_MAX_MODS] = MESHID_ALLUNK;
	uint32						count = 0;
	M2_AttachmentType		    attach[ATTACHS_MAX] = { M2_AttachmentType::Count, M2_AttachmentType::Count };
} ItemObjectComponents[size_t(EInventoryType::__Count)] =
{
	{ EInventoryType::NON_EQUIP, },
	{ EInventoryType::HEAD,          "Head",     MESHID_ALLUNK,                                                         1, M2_AttachmentType::Helm                                           },
	{ EInventoryType::NECK, },
	{ EInventoryType::SHOULDERS,     "SHOULDER", MESHID_ALLUNK,                                                         2, M2_AttachmentType::ShoulderLeft, M2_AttachmentType::ShoulderRight },
	{ EInventoryType::BODY,          "",         { MeshIDType::Wristbands, MeshIDType::Chest,    MeshIDType::UNK }                                                                           },
	{ EInventoryType::CHEST,         "",         { MeshIDType::Wristbands, MeshIDType::Chest,    MeshIDType::Trousers }                                                                      },
	{ EInventoryType::WAIST,         "",         { MeshIDType::Belt,       MeshIDType::UNK,      MeshIDType::UNK }                                                                           },
	{ EInventoryType::LEGS,          "",         { MeshIDType::Pants,      MeshIDType::Kneepads, MeshIDType::Trousers }                                                                      },
	{ EInventoryType::FEET,          "",         { MeshIDType::Boots,      MeshIDType::UNK,      MeshIDType::UNK }                                                                           },
	{ EInventoryType::WRISTS, },
	{ EInventoryType::HANDS,         "",         { MeshIDType::Glove,      MeshIDType::UNK,      MeshIDType::UNK }                                                                           },

	{ EInventoryType::FINGER, },
	{ EInventoryType::TRINKET },

	{ EInventoryType::WEAPON,        "WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandRight                                      },
	{ EInventoryType::SHIELD,        "Shield",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::Shield                                         },
	{ EInventoryType::RANGED,        "WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandRight                                      },

	{ EInventoryType::CLOAK,         "Cape",     { MeshIDType::Cloak,     MeshIDType::UNK,      MeshIDType::UNK },      1, M2_AttachmentType::Back /*Cloack specific*/                       },

	{ EInventoryType::TWOHWEAPON,    "WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandRight                                      },
	{ EInventoryType::BAG,           "Pouch" },
	{ EInventoryType::TABARD,        "",         { MeshIDType::Tabard,    MeshIDType::UNK,      MeshIDType::UNK }                                                                            },

	{ EInventoryType::ROBE, },

	{ EInventoryType::WEAPONMAINHAND,"WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandRight                                      },
	{ EInventoryType::WEAPONOFFHAND, "WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandLeft                                       },
	{ EInventoryType::HOLDABLE, },

	{ EInventoryType::AMMO,          "Ammo" },
	{ EInventoryType::THROWN, },
	{ EInventoryType::RANGEDRIGHT, },

	{ EInventoryType::QUIVER,        "Quiver" },
	{ EInventoryType::RELIC, }
};

// Textures names
struct
{
	const DBC_CharComponent_Sections list;
	const char* const folder;
} ItemTextureComponents[static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT)] =
{
	{ DBC_CharComponent_Sections::ARMS_UPPER,	"ARMUPPERTEXTURE" },
	{ DBC_CharComponent_Sections::ARMS_LOWER,	"ARMLOWERTEXTURE" },
	{ DBC_CharComponent_Sections::HANDS,		"HandTexture" },
	{ DBC_CharComponent_Sections::TORSO_UPPER,	"TorsoUpperTexture" },
	{ DBC_CharComponent_Sections::TORSO_LOWER,	"TorsoLowerTexture" },
	{ DBC_CharComponent_Sections::LEGS_UPPER,	"LEGUPPERTEXTURE" },
	{ DBC_CharComponent_Sections::LEGS_LOWER,	"LegLowerTexture" },
	{ DBC_CharComponent_Sections::FEET,			"FootTexture" }
};

CItem_VisualData::CItem_VisualData(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<Character>& Character)
	: CInet_ItemTemplate()
	, CLoadableObject(Character)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_ParentCharacter(*Character)
{
	m_DBCs = m_BaseManager.GetManager<CDBCStorage>();
}

CItem_VisualData::~CItem_VisualData()
{
}

/*CItem_VisualData::CItem_VisualData(uint32 _displayId, EInventoryType::List _inventoryType, uint32 _enchantAuraID) :
	ItemTemplate(_displayId, _inventoryType, _enchantAuraID)
{}*/

bool CItem_VisualData::Load()
{
	if (m_DisplayId == 0 || m_InventoryType == (uint8)EInventoryType::NON_EQUIP)
	{
		return false;
	}

	InitObjectComponents();
	InitGeosetComponents();
	InitTextureComponents();
	return true;
}

bool CItem_VisualData::Delete()
{
	return false;
}

	//if (m_InventoryType == EInventoryType::CLOAK)
	//{
	//	return;
	//}

	//for (auto& com : m_ObjectComponents)
	//{
		//com.model->Render3D();
	//}

void CItem_VisualData::InitObjectComponents()
{
	const DBC_ItemDisplayInfoRecord* itemDisplayInfoRecord = m_DBCs->DBC_ItemDisplayInfo()[m_DisplayId];
	if (itemDisplayInfoRecord == nullptr)
		throw CException("CItem_VisualData::InitObjectComponents: ItemDisplayInfoRecord don't contains id '%d'.", m_DisplayId);

	for (uint32 i = 0; i < ItemObjectComponents[static_cast<size_t>(m_InventoryType)].count; i++)
	{
		std::string objectFileName = itemDisplayInfoRecord->Get_ObjectModelName(i);
		std::string objectTextureName = itemDisplayInfoRecord->Get_ObjectTextureName(i);

		if (objectFileName.empty() && m_InventoryType != (uint8)EInventoryType::CLOAK)
		{
			continue;
		}

		if (m_InventoryType == (uint8)EInventoryType::HEAD)
		{
			char modelPostfix[64];
			sprintf_s(modelPostfix, "_%s%c", m_DBCs->DBC_ChrRaces()[static_cast<size_t>(m_ParentCharacter.GetTemplate().Race)]->Get_ClientPrefix().c_str(), getGenderLetter(m_ParentCharacter.GetTemplate().Gender));

			int dotPosition = objectFileName.find_last_of('.');
			_ASSERT(dotPosition != -1);
			objectFileName.insert(dotPosition, modelPostfix);
		}
		else if (m_InventoryType == (uint8)EInventoryType::CLOAK)
		{
			std::shared_ptr<IImage> texture = LoadObjectTexture((EInventoryType)m_InventoryType, objectTextureName);
			m_ObjectComponents.push_back({ nullptr, texture, M2_AttachmentType::NotAttached });
			continue;
		}

		// Fill data
		std::string modelName = GetObjectModelName((EInventoryType)m_InventoryType, objectFileName);
		std::shared_ptr<IImage> itemObjectTexture = LoadObjectTexture((EInventoryType)m_InventoryType, objectTextureName);
		auto itemObjectAttach = m_ParentCharacter.getM2().getMiscellaneous().getAttachment(ItemObjectComponents[static_cast<size_t>(m_InventoryType)].attach[i]);

		// Create instance
		std::shared_ptr<CM2> m2Model = m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, modelName);
		_ASSERT(m2Model != nullptr);

		std::shared_ptr<CItem_M2Instance> itemObjectInstance = m_ParentCharacter.CreateSceneNode<CItem_M2Instance>(m2Model);
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(itemObjectInstance);
		itemObjectInstance->Attach(itemObjectAttach.GetAttachmentType());
		itemObjectInstance->setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(itemObjectTexture));


		//
		// Visual effect
		//
		const DBC_ItemVisualsRecord* visuals = /*displayInfo->Get_ItemVisualID()*/m_DBCs->DBC_ItemVisuals()[m_EnchantAuraID];
		if (visuals != nullptr)
		{
			for (uint32 j = 0; j < DBC_ItemVisuals_VisualEffect_Count; j++)
			{
				const DBC_ItemVisualEffectsRecord* visEffect = m_DBCs->DBC_ItemVisualEffects()[visuals->Get_VisualEffect(j)];
				if (visEffect == nullptr)
					continue;

				std::string visEffectModelName = visEffect->Get_Name();
				if (visEffectModelName.empty())
					continue;

				// M2 model
				std::shared_ptr<CM2> visualEffectM2Model = m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, visEffectModelName);
				_ASSERT(visualEffectM2Model != nullptr);

				// M2 instance
				std::shared_ptr<CM2_Base_Instance> visualEffectInstance = itemObjectInstance->CreateSceneNode<CM2_Base_Instance>(visualEffectM2Model);
				m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(visualEffectInstance);

				if (itemObjectInstance->getM2().getMiscellaneous().isAttachmentExists((M2_AttachmentType)j))
					visualEffectInstance->Attach(itemObjectInstance->getM2().getMiscellaneous().getAttachment((M2_AttachmentType)j).GetAttachmentType());
				else
					visualEffectInstance->Attach(itemObjectAttach.GetAttachmentType());

				itemObjectInstance->AddVisualEffect(visualEffectInstance);
			}
		}

		m_ObjectComponents.push_back({ itemObjectInstance, itemObjectTexture, itemObjectAttach.GetAttachmentType() });
	}
}

void CItem_VisualData::InitGeosetComponents()
{
	const DBC_ItemDisplayInfoRecord* displayInfo = m_DBCs->DBC_ItemDisplayInfo()[m_DisplayId];
	_ASSERT(displayInfo != nullptr);

	for (uint32 j = 0; j < MESHID_MAX_MODS; j++)
	{
		MeshIDType mesh = ItemObjectComponents[static_cast<size_t>(m_InventoryType)].modifiers[j];
		if (mesh == MeshIDType::UNK)
			continue;

		m_GeosetComponents.push_back({ mesh, displayInfo->Get_GeosetGroups(j) });
	}
}

void CItem_VisualData::InitTextureComponents()
{
	const DBC_ItemDisplayInfoRecord* displayInfo = m_DBCs->DBC_ItemDisplayInfo()[m_DisplayId];
	_ASSERT(displayInfo != nullptr);

	for (uint32 i = 0; i < static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT); i++)
	{
		std::string textureComponentName = displayInfo->Get_TextureComponents(i);
		if (textureComponentName.empty())
			continue;

		m_TextureComponents[i] = LoadSkinTexture(ItemTextureComponents[i].list, textureComponentName);
	}
}

std::string CItem_VisualData::GetObjectModelName(EInventoryType _objectType, std::string _modelName)
{
	return "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(_objectType)].folder) + "\\" + _modelName;
}

/*std::shared_ptr<M2> CItem_VisualData::LoadObjectModel(EInventoryType::List _objectType, std::string _modelName)
{
	return GetManager<IM2Manager>()->Add("Item\\ObjectComponents\\" + ItemObjectComponents[_objectType].folder + "\\" + _modelName);
}*/

std::shared_ptr<IImage> CItem_VisualData::LoadObjectTexture(EInventoryType _objectType, std::string _textureName)
{
	std::string imageFilename = "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(_objectType)].folder) + "\\" + _textureName + ".blp";
	return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(imageFilename);
}

std::shared_ptr<IImage> CItem_VisualData::LoadSkinTexture(DBC_CharComponent_Sections _type, std::string _textureName)
{
	std::string universalTexture = getTextureComponentName(_type, _textureName, Gender::None);
	std::string maleTexture = getTextureComponentName(_type, _textureName, Gender::Male);
	std::string femaleTexture = getTextureComponentName(_type, _textureName, Gender::Female);

	IFilesManager* fManager = m_BaseManager.GetManager<IFilesManager>();

	if (fManager->IsFileExists(universalTexture))
	{
		return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(universalTexture);
	}
	else if (fManager->IsFileExists(maleTexture))
	{
		return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(maleTexture);
	}
	else if (fManager->IsFileExists(femaleTexture))
	{
		return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(femaleTexture);
	}

	return nullptr;
}



//--

std::string CItem_VisualData::getTextureComponentName(DBC_CharComponent_Sections _type, std::string _textureName, Gender _gender)
{
	char maleTexture[MAX_PATH];
	sprintf_s(maleTexture, "Item\\TEXTURECOMPONENTS\\%s\\%s_%c.blp", ItemTextureComponents[static_cast<size_t>(_type)].folder, _textureName.c_str(), getGenderLetter(_gender));
	return std::string(maleTexture);
}

char CItem_VisualData::getGenderLetter(Gender _gender)
{
	switch (_gender)
	{
		case Gender::Male: 
			return 'M';
		case Gender::Female: 
			return 'F';
		case Gender::None: 
			return 'U';
	}

	return 0x00;
}

#endif
