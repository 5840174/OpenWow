#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "World/Character/Character.h"

// General
#include "CharacterVisualItem.h"

#define MESHID_MAX_MODS 3
#define MESHID_ALLUNK { MeshIDType::UNK, MeshIDType::UNK, MeshIDType::UNK }
#define ATTACHS_MAX 2

// Object components
struct
{
	DBCItem_EInventoryItemSlot	    slot;
	const char* const			folder = "";
	MeshIDType					modifiers[MESHID_MAX_MODS] = MESHID_ALLUNK;
	uint32						count = 0;
	EM2_AttachmentType		    attach[ATTACHS_MAX] = { EM2_AttachmentType::Count, EM2_AttachmentType::Count };
} 
ItemObjectComponents[size_t(DBCItem_EInventoryItemSlot::__Count)] =
{
	{ DBCItem_EInventoryItemSlot::NON_EQUIP, },
	{ DBCItem_EInventoryItemSlot::HEAD,          "Head",     MESHID_ALLUNK,                                                         1, EM2_AttachmentType::Helm                                            },
	{ DBCItem_EInventoryItemSlot::NECK                                                                                                                                                                     },
	{ DBCItem_EInventoryItemSlot::SHOULDERS,     "SHOULDER", MESHID_ALLUNK,                                                         2, EM2_AttachmentType::ShoulderLeft, EM2_AttachmentType::ShoulderRight },
	{ DBCItem_EInventoryItemSlot::BODY,          "",         { MeshIDType::Wristbands, MeshIDType::Chest,    MeshIDType::UNK }                                                                             },
	{ DBCItem_EInventoryItemSlot::CHEST,         "",         { MeshIDType::Wristbands, MeshIDType::Chest,    MeshIDType::Trousers13 }                                                                      },
	{ DBCItem_EInventoryItemSlot::WAIST,         "",         { MeshIDType::Belt18,     MeshIDType::UNK,      MeshIDType::UNK }                                                                             },
	{ DBCItem_EInventoryItemSlot::LEGS,          "",         { MeshIDType::Pants,      MeshIDType::Kneepads, MeshIDType::Trousers13 }                                                                      },
	{ DBCItem_EInventoryItemSlot::FEET,          "",         { MeshIDType::Boots05,    MeshIDType::UNK,      MeshIDType::UNK }                                                                             },
	{ DBCItem_EInventoryItemSlot::WRISTS                                                                                                                                                                   },
	{ DBCItem_EInventoryItemSlot::HANDS,         "",         { MeshIDType::Glove04,    MeshIDType::UNK,      MeshIDType::UNK }                                                                             },

	{ DBCItem_EInventoryItemSlot::FINGER                                                                                                                                                                   },
	{ DBCItem_EInventoryItemSlot::TRINKET                                                                                                                                                                  },

	{ DBCItem_EInventoryItemSlot::WEAPON,        "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentType::HandRight                                       },
	{ DBCItem_EInventoryItemSlot::SHIELD,        "Shield",   MESHID_ALLUNK,                                                         1, EM2_AttachmentType::Shield                                          },
	{ DBCItem_EInventoryItemSlot::RANGED,        "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentType::HandRight                                       },

	{ DBCItem_EInventoryItemSlot::CLOAK,         "Cape",     { MeshIDType::Cloak15,   MeshIDType::UNK,      MeshIDType::UNK },      1, EM2_AttachmentType::Back /*Cloack specific*/                        },

	{ DBCItem_EInventoryItemSlot::TWOHWEAPON,    "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentType::HandRight                                       },
	{ DBCItem_EInventoryItemSlot::BAG,           "Pouch" },
	{ DBCItem_EInventoryItemSlot::TABARD,        "",         { MeshIDType::Tabard,    MeshIDType::UNK,      MeshIDType::UNK }                                                                              },

	{ DBCItem_EInventoryItemSlot::ROBE                                                                                                                                                                     },

	{ DBCItem_EInventoryItemSlot::WEAPONMAINHAND,"WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentType::HandRight                                       },
	{ DBCItem_EInventoryItemSlot::WEAPONOFFHAND, "WEAPON",   MESHID_ALLUNK,                                                         1, EM2_AttachmentType::HandLeft                                        },
	{ DBCItem_EInventoryItemSlot::HOLDABLE                                                                                                                                                                 },

	{ DBCItem_EInventoryItemSlot::AMMO,          "Ammo"                                                                                                                                                    },
	{ DBCItem_EInventoryItemSlot::THROWN                                                                                                                                                                   },
	{ DBCItem_EInventoryItemSlot::RANGEDRIGHT                                                                                                                                                              },

	{ DBCItem_EInventoryItemSlot::QUIVER,        "Quiver"                                                                                                                                                  },
	{ DBCItem_EInventoryItemSlot::RELIC                                                                                                                                                                    }
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



CCharacterVisualItem::CCharacterVisualItem(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<Character>& OwnerCharacter)
	: CLoadableObject(OwnerCharacter)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_OwnerCharacter(*OwnerCharacter)
{}

CCharacterVisualItem::~CCharacterVisualItem()
{}



//
// ILoadable
//
bool CCharacterVisualItem::Load()
{
	if (GetTemplate().ItemDisplayId == 0 || GetTemplate().InventoryType == (uint8)DBCItem_EInventoryItemSlot::NON_EQUIP)
		return false;

	InitObjectComponents();
	InitGeosetComponents();
	InitTextureComponents();

	m_OwnerCharacter.Refresh_AddItemsToSkinTexture();
	m_OwnerCharacter.RefreshMeshIDs();

	return true;
}

bool CCharacterVisualItem::Delete()
{
	return false;
}



//
// Private
//
void CCharacterVisualItem::InitObjectComponents()
{
	const DBC_ItemDisplayInfoRecord* itemDisplayInfoRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemDisplayInfo()[GetTemplate().ItemDisplayId];
	if (itemDisplayInfoRecord == nullptr)
		throw CException("CCharacterVisualItem::InitObjectComponents: ItemDisplayInfoRecord don't contains id '%d'.", GetTemplate().ItemDisplayId);

	for (uint32 i = 0; i < ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].count; i++)
	{
		std::string objectFileName = itemDisplayInfoRecord->Get_ObjectModelName(i);
		std::string objectTextureName = itemDisplayInfoRecord->Get_ObjectTextureName(i);

		if (objectFileName.empty() && GetTemplate().InventoryType != (uint8)DBCItem_EInventoryItemSlot::CLOAK)
			continue;

		if (GetTemplate().InventoryType == (uint8)DBCItem_EInventoryItemSlot::HEAD)
		{
			char modelPostfix[64];
			sprintf_s(modelPostfix, "_%s%c", m_BaseManager.GetManager<CDBCStorage>()->DBC_ChrRaces()[static_cast<size_t>(m_OwnerCharacter.GetTemplate().Race)]->Get_ClientPrefix().c_str(), getGenderLetter(m_OwnerCharacter.GetTemplate().Gender));

			int dotPosition = objectFileName.find_last_of('.');
			_ASSERT(dotPosition != -1);
			objectFileName.insert(dotPosition, modelPostfix);
		}
		else if (GetTemplate().InventoryType == (uint8)DBCItem_EInventoryItemSlot::CLOAK)
		{
			auto cloackImage = LoadObjectTexture(DBCItem_EInventoryItemSlot::CLOAK, objectTextureName);
			m_ObjectComponents.push_back({ nullptr, cloackImage, EM2_AttachmentType::NotAttached });
			continue;
		}


		// Create model and node for Item
		auto itemModelFilename = GetObjectModelName((DBCItem_EInventoryItemSlot)GetTemplate().InventoryType, objectFileName);
		auto itemModel = m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, itemModelFilename);
		_ASSERT(itemModel != nullptr);

		auto itemModelInstance = m_OwnerCharacter.CreateSceneNode<CItem_M2Instance>(itemModel);
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(itemModelInstance);

		auto ownerCharacterAttachmentPoint = m_OwnerCharacter.getM2().getMiscellaneous().getAttachment(ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].attach[i]);
		itemModelInstance->Attach(ownerCharacterAttachmentPoint->GetAttachmentType());

		auto itemImage = LoadObjectTexture((DBCItem_EInventoryItemSlot)GetTemplate().InventoryType, objectTextureName);
		auto itemTexture = m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(itemImage);
		itemModelInstance->setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, itemTexture);


		// Visual effect
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
					throw CException("CCharacterVisualItem::InitObjectComponents: itemVisualEffectM2Model is nullptr. ItemVisualsRecord ID = '%d', ItemVisualEffectsRecord = '%d' Index = '%d', Path '%s'.", GetTemplate().EnchantAuraID, itemVisualEffectID, j, itemVisualEffectRecordModelFilename.c_str());

				// M2 Instance
				auto itemVisualEffectInstance = itemModelInstance->CreateSceneNode<CM2_Base_Instance>(itemVisualEffectModel);
				m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(itemVisualEffectInstance);

				if (itemModelInstance->getM2().getMiscellaneous().isAttachmentExists((EM2_AttachmentType)j))
				{
					auto itemModelAttachmentPoint = itemModelInstance->getM2().getMiscellaneous().getAttachment((EM2_AttachmentType)j);
					itemVisualEffectInstance->Attach(itemModelAttachmentPoint->GetAttachmentType()); // Attach visual effect to itemInstace attachment
				}
				else
				{
					itemVisualEffectInstance->Attach(ownerCharacterAttachmentPoint->GetAttachmentType()); // Or attach visual effect to same attachment as item
				}

				itemModelInstance->AddVisualEffect(itemVisualEffectInstance);
			}
		}

		m_ObjectComponents.push_back({ itemModelInstance, itemImage, ownerCharacterAttachmentPoint->GetAttachmentType() });
	}
}

void CCharacterVisualItem::InitGeosetComponents()
{
	const DBC_ItemDisplayInfoRecord* itemDisplayInfoRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemDisplayInfo()[GetTemplate().ItemDisplayId];
	if (itemDisplayInfoRecord == nullptr)
		throw CException("CCharacterVisualItem::InitGeosetComponents: ItemDisplayInfoRecord don't contains id '%d'.", GetTemplate().ItemDisplayId);

	for (uint32 j = 0; j < MESHID_MAX_MODS; j++)
	{
		MeshIDType mesh = ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].modifiers[j];
		if (mesh == MeshIDType::UNK)
			continue;

		m_GeosetComponents.push_back({ mesh, itemDisplayInfoRecord->Get_GeosetGroups(j) });
	}
}

void CCharacterVisualItem::InitTextureComponents()
{
	const DBC_ItemDisplayInfoRecord* itemDisplayInfoRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemDisplayInfo()[GetTemplate().ItemDisplayId];
	if (itemDisplayInfoRecord == nullptr)
		throw CException("CCharacterVisualItem::InitGeosetComponents: ItemDisplayInfoRecord don't contains id '%d'.", GetTemplate().ItemDisplayId);

	for (uint32 i = 0; i < static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT); i++)
	{
		std::string textureComponentName = itemDisplayInfoRecord->Get_TextureComponents(i);
		if (textureComponentName.empty())
			continue;

		m_TextureComponents[i] = LoadSkinTexture(ItemTextureComponents[i].list, textureComponentName);
	}
}

std::string CCharacterVisualItem::GetObjectModelName(DBCItem_EInventoryItemSlot ObjectType, std::string _modelName)
{
	return "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(ObjectType)].folder) + "\\" + _modelName;
}

std::shared_ptr<IImage> CCharacterVisualItem::LoadObjectTexture(DBCItem_EInventoryItemSlot _objectType, std::string _textureName)
{
	std::string imageFilename = "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(_objectType)].folder) + "\\" + _textureName + ".blp";
	return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(imageFilename);
}

std::shared_ptr<IImage> CCharacterVisualItem::LoadSkinTexture(DBC_CharComponent_Sections _type, std::string _textureName)
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
