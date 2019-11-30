#include "stdafx.h"

// Include
#include "Character.h"
#include "Creature_M2Instance.h"

// General
#include "Item_VisualData.h"

#define MESHID_MAX_MODS 3
#define MESHID_ALLUNK { MeshIDType::UNK, MeshIDType::UNK, MeshIDType::UNK }
#define ATTACHS_MAX 2

// Object components
struct
{
	InventoryType::List	        slot;
	std::string					folder = "";
	MeshIDType::List			modifiers[MESHID_MAX_MODS] = MESHID_ALLUNK;
	uint32						count = 0;
	M2_AttachmentType::List		attach[ATTACHS_MAX] = { M2_AttachmentType::Count, M2_AttachmentType::Count };
} ItemObjectComponents[InventoryType::COUNT] =
{
	{ InventoryType::NON_EQUIP, },
	{ InventoryType::HEAD,          "Head",     MESHID_ALLUNK,                                                         1, M2_AttachmentType::Helm                                           },
	{ InventoryType::NECK, },
	{ InventoryType::SHOULDERS,     "SHOULDER", MESHID_ALLUNK,                                                         2, M2_AttachmentType::ShoulderLeft, M2_AttachmentType::ShoulderRight },
	{ InventoryType::BODY,          "",         { MeshIDType::Wristbands, MeshIDType::Chest,    MeshIDType::UNK }                                                                           },
	{ InventoryType::CHEST,         "",         { MeshIDType::Wristbands, MeshIDType::Chest,    MeshIDType::Trousers }                                                                      },
	{ InventoryType::WAIST,         "",         { MeshIDType::Belt,       MeshIDType::UNK,      MeshIDType::UNK }                                                                           },
	{ InventoryType::LEGS,          "",         { MeshIDType::Pants,      MeshIDType::Kneepads, MeshIDType::Trousers }                                                                      },
	{ InventoryType::FEET,          "",         { MeshIDType::Boots,      MeshIDType::UNK,      MeshIDType::UNK }                                                                           },
	{ InventoryType::WRISTS, },
	{ InventoryType::HANDS,         "",         { MeshIDType::Glove,      MeshIDType::UNK,      MeshIDType::UNK }                                                                           },

	{ InventoryType::FINGER, },
	{ InventoryType::TRINKET },

	{ InventoryType::WEAPON,        "WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandRight                                      },
	{ InventoryType::SHIELD,        "Shield",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::Shield                                         },
	{ InventoryType::RANGED,        "WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandRight                                      },

	{ InventoryType::CLOAK,         "Cape",     { MeshIDType::Cloak,     MeshIDType::UNK,      MeshIDType::UNK },      1, M2_AttachmentType::Back /*Cloack specific*/                       },

	{ InventoryType::TWOHWEAPON,    "WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandRight                                      },
	{ InventoryType::BAG,           "Pouch" },
	{ InventoryType::TABARD,        "",         { MeshIDType::Tabard,    MeshIDType::UNK,      MeshIDType::UNK }                                                                            },

	{ InventoryType::ROBE, },

	{ InventoryType::WEAPONMAINHAND,"WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandRight                                      },
	{ InventoryType::WEAPONOFFHAND, "WEAPON",   MESHID_ALLUNK,                                                         1, M2_AttachmentType::HandLeft                                       },
	{ InventoryType::HOLDABLE, },

	{ InventoryType::AMMO,          "Ammo" },
	{ InventoryType::THROWN, },
	{ InventoryType::RANGEDRIGHT, },

	{ InventoryType::QUIVER,        "Quiver" },
	{ InventoryType::RELIC, }
};

// Textures names
struct
{
	DBC_CharComponent_Sections::List list;
	std::string folder;
} ItemTextureComponents[DBC_CharComponent_Sections::ITEMS_COUNT] =
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

CItem_VisualData::CItem_VisualData(std::shared_ptr<Character> _owner) :
	ItemTemplate(),
	m_ParentCharacter(_owner)
{}

/*CItem_VisualData::CItem_VisualData(uint32 _displayId, InventoryType::List _inventoryType, uint32 _enchantAuraID) :
	ItemTemplate(_displayId, _inventoryType, _enchantAuraID)
{}*/

void CItem_VisualData::Load()
{
	if (DisplayId == 0 || InventoryType == InventoryType::NON_EQUIP)
	{
		return;
	}

	InitObjectComponents();
	InitGeosetComponents();
	InitTextureComponents();
}

void CItem_VisualData::Render3D()
{
	if (InventoryType == InventoryType::CLOAK)
	{
		return;
	}

	for (auto& com : m_ObjectComponents)
	{
		//com.model->Render3D();
	}
}

void CItem_VisualData::InitObjectComponents()
{
	std::shared_ptr<const DBC_ItemDisplayInfoRecord> displayInfo = DBC_ItemDisplayInfo[DisplayId];
	_ASSERT(displayInfo != nullptr);

	for (uint32 i = 0; i < ItemObjectComponents[InventoryType].count; i++)
	{
		std::string objectFileName = displayInfo->Get_ObjectModelName(i);
		std::string objectTextureName = displayInfo->Get_ObjectTextureName(i);

		if (objectFileName.empty() && InventoryType != InventoryType::CLOAK)
		{
			continue;
		}

		if (InventoryType == InventoryType::HEAD)
		{
			char modelPostfix[64];
			sprintf_s(modelPostfix, "_%s%c", DBC_ChrRaces[m_ParentCharacter.lock()->GetTemplate().Race]->Get_ClientPrefix(), getGenderLetter(m_ParentCharacter.lock()->GetTemplate().Gender));

			int dotPosition = objectFileName.find_last_of('.');
			_ASSERT(dotPosition != -1);
			objectFileName.insert(dotPosition, modelPostfix);
		}
		else if (InventoryType == InventoryType::CLOAK)
		{
			std::shared_ptr<ITexture> texture = LoadObjectTexture(InventoryType, objectTextureName);
			m_ObjectComponents.push_back({ nullptr, texture, nullptr });
			continue;
		}

		// Fill data
		std::string modelName = GetObjectModelName(InventoryType, objectFileName);
		std::shared_ptr<ITexture> itemObjectTexture = LoadObjectTexture(InventoryType, objectTextureName);
		std::shared_ptr<CM2_Part_Attachment> itemObjectAttach = m_ParentCharacter.lock()->getM2()->getMiscellaneous()->getAttachment(ItemObjectComponents[InventoryType].attach[i]);

		// Create instance
		std::shared_ptr<CItem_M2Instance> itemObjectInstance = m_ParentCharacter.lock()->CreateSceneNode<CItem_M2Instance>(modelName);
		Application::Get().GetLoader()->AddToLoadQueue(itemObjectInstance);
		itemObjectInstance->Attach(itemObjectAttach);
		itemObjectInstance->setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, itemObjectTexture);


		// Visual effect
		std::shared_ptr<const DBC_ItemVisualsRecord> visuals = /*displayInfo->Get_ItemVisualID()*/DBC_ItemVisuals[EnchantAuraID];
		if (visuals != nullptr)
		{
			for (uint32 j = 0; j < DBC_ItemVisuals_VisualEffect_Count; j++)
			{
				std::shared_ptr<const DBC_ItemVisualEffectsRecord> visEffect = visuals->Get_VisualEffect(j);
				if (visEffect == nullptr)
				{
					continue;
				}

				std::string visEffectModelName = visEffect->Get_Name();
				if (visEffectModelName.empty())
				{
					continue;
				}

				std::shared_ptr<CM2_Base_Instance> visInstance = itemObjectInstance->CreateSceneNode<CM2_Base_Instance>(visEffectModelName);
				Application::Get().GetLoader()->AddToLoadQueue(visInstance);

				std::shared_ptr<CM2_Part_Attachment> visAttach = nullptr;

				if (itemObjectInstance->getM2()->getMiscellaneous()->isAttachmentExists((M2_AttachmentType::List)j))
				{
					visAttach = itemObjectInstance->getM2()->getMiscellaneous()->getAttachment((M2_AttachmentType::List)j);
				}
				else
				{
					visAttach = itemObjectAttach;
				}

				visInstance->Attach(visAttach);
				itemObjectInstance->AddVisualEffect(visInstance);
			}
		}

		m_ObjectComponents.push_back({ itemObjectInstance, itemObjectTexture, itemObjectAttach });
	}
}

void CItem_VisualData::InitGeosetComponents()
{
	std::shared_ptr<const DBC_ItemDisplayInfoRecord> displayInfo = DBC_ItemDisplayInfo[DisplayId];
	_ASSERT(displayInfo != nullptr);

	for (uint32 j = 0; j < 3; j++)
	{
		MeshIDType::List mesh = ItemObjectComponents[InventoryType].modifiers[j];
		if (mesh == MeshIDType::UNK)
		{
			continue;
		}

		m_GeosetComponents.push_back({ mesh, displayInfo->Get_GeosetGroups(j) });
	}
}

void CItem_VisualData::InitTextureComponents()
{
	std::shared_ptr<DBC_ItemDisplayInfoRecord> displayInfo = DBC_ItemDisplayInfo[DisplayId];
	_ASSERT(displayInfo != nullptr);

	for (uint32 i = 0; i < DBC_CharComponent_Sections::ITEMS_COUNT; i++)
	{
		std::string textureComponentName = displayInfo->Get_TextureComponents(i);
		if (textureComponentName.empty())
		{
			continue;
		}

		std::shared_ptr<ITexture> textureComponent = LoadSkinTexture(ItemTextureComponents[i].list, textureComponentName);
		m_TextureComponents[i] = textureComponent;
	}
}

std::string CItem_VisualData::GetObjectModelName(InventoryType::List _objectType, std::string _modelName)
{
	return "Item\\ObjectComponents\\" + ItemObjectComponents[_objectType].folder + "\\" + _modelName;
}

/*std::shared_ptr<M2> CItem_VisualData::LoadObjectModel(InventoryType::List _objectType, std::string _modelName)
{
	return GetManager<IM2Manager>()->Add("Item\\ObjectComponents\\" + ItemObjectComponents[_objectType].folder + "\\" + _modelName);
}*/

std::shared_ptr<ITexture> CItem_VisualData::LoadObjectTexture(InventoryType::List _objectType, std::string _textureName)
{
	return _RenderDevice->CreateTexture2D("Item\\ObjectComponents\\" + ItemObjectComponents[_objectType].folder + "\\" + _textureName + ".blp");
}

std::shared_ptr<ITexture> CItem_VisualData::LoadSkinTexture(DBC_CharComponent_Sections::List _type, std::string _textureName)
{
	std::string universalTexture = getTextureComponentName(_type, _textureName, Gender::None);
	std::string maleTexture = getTextureComponentName(_type, _textureName, Gender::Male);
	std::string femaleTexture = getTextureComponentName(_type, _textureName, Gender::Female);

	std::shared_ptr<IFilesManager> fManager = GetManager<IFilesManager>(_ApplicationInstance->GetBaseManager());

	if (fManager->IsFileExists(universalTexture))
	{
		return _RenderDevice->CreateTexture2D(universalTexture);
	}
	else if (fManager->IsFileExists(maleTexture))
	{
		return _RenderDevice->CreateTexture2D(maleTexture);
	}
	else if (fManager->IsFileExists(femaleTexture))
	{
		return _RenderDevice->CreateTexture2D(femaleTexture);
	}

	return nullptr;
}



//--

std::string CItem_VisualData::getTextureComponentName(DBC_CharComponent_Sections::List _type, std::string _textureName, Gender::List _gender)
{


	char maleTexture[256];
	sprintf_s(maleTexture, "Item\\TEXTURECOMPONENTS\\%s\\%s_%c.blp", ItemTextureComponents[_type].folder.c_str(), _textureName.c_str(), getGenderLetter(_gender));
	return std::string(maleTexture);
}

char CItem_VisualData::getGenderLetter(Gender::List _gender)
{
	switch (_gender)
	{
	case Gender::Male: return 'M';
	case Gender::Female: return 'F';
	case Gender::None: return 'U';
	}

	return '\0';
}
