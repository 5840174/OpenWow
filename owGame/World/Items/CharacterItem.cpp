#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "World/Character/Character.h"

// General
#include "CharacterItem.h"

// Additional
#include "CharacterItemEffectM2Instance.h"

namespace
{
	char GetGenderLetter(Gender Gender)
	{
		return (Gender == Gender::Male) ? 'M' : (Gender == Gender::Female) ? 'F' : 'U';
	}

	std::string GetSkinComponentImageFileName(DBC_CharComponent_Sections _type, std::string _textureName, Gender _gender)
	{
		char maleTexture[MAX_PATH];
		sprintf_s(maleTexture, "Item\\TEXTURECOMPONENTS\\%s\\%s_%c.blp", ItemTextureComponents[static_cast<size_t>(_type)].TexturesFolder, _textureName.c_str(), GetGenderLetter(_gender));
		return std::string(maleTexture);
	}
}



CCharacterItem::CCharacterItem(const IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<CCharacter>& OwnerCharacter)
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
			char genderLetter = GetGenderLetter(m_OwnerCharacter.GetTemplate().Gender);

			char modelPostfix[MAX_PATH];
			sprintf_s(modelPostfix, "_%s%c", raceClientPrefixString.c_str(), genderLetter);

			size_t dotPosition = objectFileName.find_last_of('.');
			_ASSERT(dotPosition != -1);
			objectFileName.insert(dotPosition, modelPostfix);
		}
		else if (GetTemplate().InventoryType == (uint8)DBCItem_EInventoryItemType::CLOAK)
		{
			auto cloackImage = LoadItemImage(DBCItem_EInventoryItemType::CLOAK, objectTextureName);
			m_Models.push_back({ nullptr, cloackImage });
			continue;
		}


		// Create model and node for Item
		auto itemModel = LoadItemM2Model(static_cast<DBCItem_EInventoryItemType>(GetTemplate().InventoryType), objectFileName);
		if (itemModel == nullptr)
		{
			Log::Error("CCharacterItem::InitializeItemModels: M2Model for item DisplayID '%d' not found.", GetTemplate().DisplayId);
			continue;
		}

		auto itemModelInstance = m_OwnerCharacter.CreateSceneNode<CCharacterItemM2Instance>(itemModel, *this, ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].AttachmentPoint[i]);
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(itemModelInstance);

		auto itemImage = LoadItemImage((DBCItem_EInventoryItemType)GetTemplate().InventoryType, objectTextureName);
		itemModelInstance->setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(itemImage));

		m_Models.push_back({ itemModelInstance, itemImage });
	}
}

void CCharacterItem::InitializeItemGeosets()
{
	const DBC_ItemDisplayInfoRecord* itemDisplayInfoRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemDisplayInfo()[GetTemplate().DisplayId];
	if (itemDisplayInfoRecord == nullptr)
		throw CException("CCharacterItem::InitializeItemGeosets: ItemDisplayInfoRecord don't contains id '%d'.", GetTemplate().DisplayId);

	for (uint32 j = 0; j < MESHID_MAX_MODS; j++)
	{
		EM2GeosetType mesh = ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].Geosets[j];
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

		m_SkinComponentImages[i] = LoadItemSkinComponentImage(ItemTextureComponents[i].list, textureComponentName);
	}
}

std::shared_ptr<CM2> CCharacterItem::LoadItemM2Model(DBCItem_EInventoryItemType InventoryItemType, std::string _modelName)
{
	std::string itemM2ModelFileName =  "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(InventoryItemType)].TexturesFolder) + "\\" + _modelName;
	return m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, itemM2ModelFileName);
}

std::shared_ptr<IImage> CCharacterItem::LoadItemImage(DBCItem_EInventoryItemType InventoryItemType, std::string _textureName)
{
	std::string imageFilename = "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(InventoryItemType)].TexturesFolder) + "\\" + _textureName + ".blp";
	return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(imageFilename);
}

std::shared_ptr<IImage> CCharacterItem::LoadItemSkinComponentImage(DBC_CharComponent_Sections SkinComponent, std::string _textureName)
{
	std::string maleTexture = GetSkinComponentImageFileName(SkinComponent, _textureName, Gender::Male);
	std::string femaleTexture = GetSkinComponentImageFileName(SkinComponent, _textureName, Gender::Female);
	std::string universalTexture = GetSkinComponentImageFileName(SkinComponent, _textureName, Gender::None);

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
