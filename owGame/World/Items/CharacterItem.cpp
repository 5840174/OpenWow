#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "World/Character/Character.h"

// General
#include "CharacterItem.h"

// Additional
#include "CharacterItemEffectM2Instance.h"

#define MESHID_MAX_MODS 3
#define MESHID_ALLUNK { EM2GeosetType::UNK, EM2GeosetType::UNK, EM2GeosetType::UNK }
#define ATTACHS_MAX 2

// Object components
struct
{
	DBCItem_EInventoryItemType	ItemType;
	const char* const			folder = "";
	EM2GeosetType					modifiers[MESHID_MAX_MODS] = MESHID_ALLUNK;
	uint32						AttachmentsCount = 0;
	EM2_AttachmentPoint		    AttachmentPoint[ATTACHS_MAX] = { EM2_AttachmentPoint::Count, EM2_AttachmentPoint::Count };
} 
ItemObjectComponents[size_t(DBCItem_EInventoryItemType::__Count)] =
{
	{ DBCItem_EInventoryItemType::NON_EQUIP, },
	{ DBCItem_EInventoryItemType::HEAD,          "Head",     MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::Helm                                            },
	{ DBCItem_EInventoryItemType::NECK                                                                                                                                                                     },
	{ DBCItem_EInventoryItemType::SHOULDERS,     "SHOULDER", MESHID_ALLUNK,                                                         2, EM2_AttachmentPoint::ShoulderLeft, EM2_AttachmentPoint::ShoulderRight },
	{ DBCItem_EInventoryItemType::BODY,          "",         { EM2GeosetType::Wristbands, EM2GeosetType::Chest,    EM2GeosetType::UNK }                                                                             },
	{ DBCItem_EInventoryItemType::CHEST,         "",         { EM2GeosetType::Wristbands, EM2GeosetType::Chest,    EM2GeosetType::Trousers13 }                                                                      },
	{ DBCItem_EInventoryItemType::WAIST,         "",         { EM2GeosetType::Belt18,     EM2GeosetType::UNK,      EM2GeosetType::UNK }                                                                             },
	{ DBCItem_EInventoryItemType::LEGS,          "",         { EM2GeosetType::Pants,      EM2GeosetType::Kneepads, EM2GeosetType::Trousers13 }                                                                      },
	{ DBCItem_EInventoryItemType::FEET,          "",         { EM2GeosetType::Boots05,    EM2GeosetType::UNK,      EM2GeosetType::UNK }                                                                             },
	{ DBCItem_EInventoryItemType::WRISTS                                                                                                                                                                   },
	{ DBCItem_EInventoryItemType::HANDS,         "",         { EM2GeosetType::Glove04,    EM2GeosetType::UNK,      EM2GeosetType::UNK }                                                                             },

	{ DBCItem_EInventoryItemType::FINGER                                                                                                                                                                   },
	{ DBCItem_EInventoryItemType::TRINKET                                                                                                                                                                  },

	{ DBCItem_EInventoryItemType::WEAPON,        "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandRight                                       },
	{ DBCItem_EInventoryItemType::SHIELD,        "Shield",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::Shield                                          },
	{ DBCItem_EInventoryItemType::RANGED,        "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandRight                                       },

	{ DBCItem_EInventoryItemType::CLOAK,         "Cape",     { EM2GeosetType::Cloak15,   EM2GeosetType::UNK,      EM2GeosetType::UNK },      1, EM2_AttachmentPoint::Back /*Cloack specific*/                        },

	{ DBCItem_EInventoryItemType::TWOHWEAPON,    "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandRight                                       },
	{ DBCItem_EInventoryItemType::BAG,           "Pouch" },
	{ DBCItem_EInventoryItemType::TABARD,        "",         { EM2GeosetType::Tabard,    EM2GeosetType::UNK,      EM2GeosetType::UNK }                                                                              },

	{ DBCItem_EInventoryItemType::ROBE                                                                                                                                                                     },

	{ DBCItem_EInventoryItemType::WEAPONMAINHAND,"WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandRight                                       },
	{ DBCItem_EInventoryItemType::WEAPONOFFHAND, "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentPoint::HandLeft                                        },
	{ DBCItem_EInventoryItemType::HOLDABLE                                                                                                                                                                 },

	{ DBCItem_EInventoryItemType::AMMO,          "Ammo"                                                                                                                                                    },
	{ DBCItem_EInventoryItemType::THROWN                                                                                                                                                                   },
	{ DBCItem_EInventoryItemType::RANGEDRIGHT                                                                                                                                                              },

	{ DBCItem_EInventoryItemType::QUIVER,        "Quiver"                                                                                                                                                  },
	{ DBCItem_EInventoryItemType::RELIC                                                                                                                                                                    }
};

// Textures names
struct
{
	const DBC_CharComponent_Sections list;
	const char* const folder;
} 
ItemTextureComponents[static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT)] =
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



namespace
{
	char getGenderLetter(Gender Gender)
	{
		return (Gender == Gender::Male) ? 'M' : (Gender == Gender::Female) ? 'F' : 'U';
	}

	std::string getTextureComponentName(DBC_CharComponent_Sections _type, std::string _textureName, Gender _gender)
	{
		char maleTexture[MAX_PATH];
		sprintf_s(maleTexture, "Item\\TEXTURECOMPONENTS\\%s\\%s_%c.blp", ItemTextureComponents[static_cast<size_t>(_type)].folder, _textureName.c_str(), getGenderLetter(_gender));
		return std::string(maleTexture);
	}
}



CCharacterItem::CCharacterItem(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<CCharacter>& OwnerCharacter)
	: CLoadableObject(OwnerCharacter)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_OwnerCharacter(*OwnerCharacter)
{}

CCharacterItem::~CCharacterItem()
{}



//
// ILoadable
//
bool CCharacterItem::Load()
{
	if (GetTemplate().DisplayId == 0 || GetTemplate().InventoryType == (uint8)DBCItem_EInventoryItemType::NON_EQUIP)
		return false;

	InitializeItemModels();
	InitializeItemGeosets();
	InitializeItemSkinImages();

	m_OwnerCharacter.Refresh_AddItemsToSkinTexture();
	m_OwnerCharacter.RefreshMeshIDs();

	return true;
}

bool CCharacterItem::Delete()
{
	return false;
}



//
// Private
//
void CCharacterItem::InitializeItemModels()
{
	const DBC_ItemDisplayInfoRecord* itemDisplayInfoRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemDisplayInfo()[GetTemplate().DisplayId];
	if (itemDisplayInfoRecord == nullptr)
		throw CException("CCharacterItem::InitializeItemModels: ItemDisplayInfoRecord don't contains id '%d'.", GetTemplate().DisplayId);

	for (uint32 i = 0; i < ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].AttachmentsCount; i++)
	{
		std::string objectFileName = itemDisplayInfoRecord->Get_ObjectModelName(i);
		std::string objectTextureName = itemDisplayInfoRecord->Get_ObjectTextureName(i);

		if (objectFileName.empty() && GetTemplate().InventoryType != (uint8)DBCItem_EInventoryItemType::CLOAK)
			continue;

		if (GetTemplate().InventoryType == (uint8)DBCItem_EInventoryItemType::HEAD)
		{
			std::string raceClientPrefixString = m_BaseManager.GetManager<CDBCStorage>()->DBC_ChrRaces()[static_cast<size_t>(m_OwnerCharacter.GetTemplate().Race)]->Get_ClientPrefix();
			char genderLetter = getGenderLetter(m_OwnerCharacter.GetTemplate().Gender);

			char modelPostfix[MAX_PATH];
			sprintf_s(modelPostfix, "_%s%c", raceClientPrefixString.c_str(), genderLetter);

			size_t dotPosition = objectFileName.find_last_of('.');
			_ASSERT(dotPosition != -1);
			objectFileName.insert(dotPosition, modelPostfix);
		}
		else if (GetTemplate().InventoryType == (uint8)DBCItem_EInventoryItemType::CLOAK)
		{
			auto cloackImage = LoadItemImage(DBCItem_EInventoryItemType::CLOAK, objectTextureName);
			m_Models.push_back({ nullptr, cloackImage, EM2_AttachmentPoint::NotAttached });
			continue;
		}


		// Create model and node for Item
		auto itemModel = LoadItemM2Model((DBCItem_EInventoryItemType)GetTemplate().InventoryType, objectFileName);
		if (itemModel == nullptr)
		{
			Log::Error("CCharacterItem::InitializeItemModels: M2Model for item DisplayID '%d' not found.", GetTemplate().DisplayId);
			continue;
		}

		auto itemModelInstance = m_OwnerCharacter.CreateSceneNode<CCharacterItemM2Instance>(itemModel);
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(itemModelInstance);

		auto ownerCharacterAttachment = m_OwnerCharacter.getM2().getMiscellaneous().getAttachment(ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].AttachmentPoint[i]);
		auto ownerCharacterAttachmentPoint = ownerCharacterAttachment->GetAttachmentType();
		itemModelInstance->Attach(ownerCharacterAttachmentPoint);

		auto itemImage = LoadItemImage((DBCItem_EInventoryItemType)GetTemplate().InventoryType, objectTextureName);
		auto itemTexture = m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(itemImage);
		itemModelInstance->setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, itemTexture);


		// Item enchants
		const DBC_ItemVisualsRecord* itemVisualsRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemVisuals()[GetTemplate().EnchantAuraID];
		if (itemVisualsRecord != nullptr)
		{
			for (uint32 j = 0; j < DBC_ItemVisuals_VisualEffect_Count; j++)
			{
				uint32 itemVisualEffectID = itemVisualsRecord->Get_VisualEffect(j);
				const DBC_ItemVisualEffectsRecord* itemVisualEffectRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemVisualEffects()[itemVisualEffectID];
				if (itemVisualEffectRecord == nullptr)
					continue;

				std::string itemVisualEffectRecordModelFilename = itemVisualEffectRecord->Get_Name();
				if (itemVisualEffectRecordModelFilename.empty())
					continue;

				// M2 model
				auto itemVisualEffectModel = m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, itemVisualEffectRecordModelFilename);
				if (itemVisualEffectModel == nullptr)
					throw CException("CCharacterItem::InitializeItemModels: itemVisualEffectM2Model is nullptr. ItemVisualsRecord ID = '%d', ItemVisualEffectsRecord = '%d' Index = '%d', Path '%s'.", GetTemplate().EnchantAuraID, itemVisualEffectID, j, itemVisualEffectRecordModelFilename.c_str());

				// M2 Instance
				auto itemVisualEffectInstance = itemModelInstance->CreateSceneNode<CCharacterItemEffectM2Instance>(itemVisualEffectModel);
				m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(itemVisualEffectInstance);

				if (itemModelInstance->getM2().getMiscellaneous().isAttachmentExists((EM2_AttachmentPoint)j))
				{
					auto itemModelAttachmentPoint = itemModelInstance->getM2().getMiscellaneous().getAttachment((EM2_AttachmentPoint)j);
					itemVisualEffectInstance->Attach(itemModelAttachmentPoint->GetAttachmentType()); // Attach visual effect to itemInstace attachment
				}
				else
				{
					itemVisualEffectInstance->Attach(ownerCharacterAttachmentPoint); // Or attach visual effect to same attachment as item
				}

				itemModelInstance->AddVisualEffect(itemVisualEffectInstance);
			}
		}

		m_Models.push_back({ itemModelInstance, itemImage, ownerCharacterAttachmentPoint });
	}
}

void CCharacterItem::InitializeItemGeosets()
{
	const DBC_ItemDisplayInfoRecord* itemDisplayInfoRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemDisplayInfo()[GetTemplate().DisplayId];
	if (itemDisplayInfoRecord == nullptr)
		throw CException("CCharacterItem::InitializeItemGeosets: ItemDisplayInfoRecord don't contains id '%d'.", GetTemplate().DisplayId);

	for (uint32 j = 0; j < MESHID_MAX_MODS; j++)
	{
		EM2GeosetType mesh = ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].modifiers[j];
		if (mesh == EM2GeosetType::UNK)
			continue;

		m_Geosets.push_back({ mesh, itemDisplayInfoRecord->Get_GeosetGroups(j) });
	}
}

void CCharacterItem::InitializeItemSkinImages()
{
	const DBC_ItemDisplayInfoRecord* itemDisplayInfoRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemDisplayInfo()[GetTemplate().DisplayId];
	if (itemDisplayInfoRecord == nullptr)
		throw CException("CCharacterItem::InitializeItemSkinImages: ItemDisplayInfoRecord don't contains id '%d'.", GetTemplate().DisplayId);

	for (uint32 i = 0; i < static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT); i++)
	{
		std::string textureComponentName = itemDisplayInfoRecord->Get_TextureComponents(i);
		if (textureComponentName.empty())
			continue;

		m_SkinTextures[i] = LoadSkinTexture(ItemTextureComponents[i].list, textureComponentName);
	}
}

std::shared_ptr<CM2> CCharacterItem::LoadItemM2Model(DBCItem_EInventoryItemType ObjectType, std::string _modelName)
{
	std::string itemM2ModelFileName =  "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(ObjectType)].folder) + "\\" + _modelName;
	return m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, itemM2ModelFileName);
}

std::shared_ptr<IImage> CCharacterItem::LoadItemImage(DBCItem_EInventoryItemType _objectType, std::string _textureName)
{
	std::string imageFilename = "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(_objectType)].folder) + "\\" + _textureName + ".blp";
	return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(imageFilename);
}

std::shared_ptr<IImage> CCharacterItem::LoadSkinTexture(DBC_CharComponent_Sections _type, std::string _textureName)
{
	std::string maleTexture = getTextureComponentName(_type, _textureName, Gender::Male);
	std::string femaleTexture = getTextureComponentName(_type, _textureName, Gender::Female);
	std::string universalTexture = getTextureComponentName(_type, _textureName, Gender::None);

	IFilesManager* filesManager = m_BaseManager.GetManager<IFilesManager>();

	if (m_OwnerCharacter.GetTemplate().Gender == Gender::Male && filesManager->IsFileExists(maleTexture))
	{
		return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(maleTexture);
	}
	if (m_OwnerCharacter.GetTemplate().Gender == Gender::Female && filesManager->IsFileExists(femaleTexture))
	{
		return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(femaleTexture);
	}
	else if (filesManager->IsFileExists(universalTexture))
	{
		return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(universalTexture);
	}
	
	return nullptr;
}

#endif
