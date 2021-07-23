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
		sprintf_s(maleTexture, "Item\\TEXTURECOMPONENTS\\%s\\%s_%c.blp", ItemTextureComponents[static_cast<size_t>(_type)].ModelsAndTexturesFolder, _textureName.c_str(), GetGenderLetter(_gender));
		return std::string(maleTexture);
	}
}



CCharacterItem::CCharacterItem(const IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<CCharacter>& OwnerCharacter, const SCharacterItemTemplate& CharacterItemTemplate)
	: CLoadableObject(OwnerCharacter)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_OwnerCharacter(*OwnerCharacter)
	, m_Template(CharacterItemTemplate)
	, m_ItemDisplayInfoRecord(nullptr)
	, m_HelmetGeosetVisDataRecord(nullptr)
{
	if (false == IsExists())
		return;

	m_ItemDisplayInfoRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_ItemDisplayInfo()[GetTemplate().DisplayId];
	if (m_ItemDisplayInfoRecord == nullptr)
		throw CException("CCharacterItem::CCharacterItem: DBC_ItemDisplayInfo don't contains id '%d'.", GetTemplate().DisplayId);

	uint32 geosetVisualDataID = 0;
	if (m_OwnerCharacter.GetTemplate().Gender == Gender::Male)
		geosetVisualDataID = m_ItemDisplayInfoRecord->Get_HelmetGeosetMaleID();
	else if (m_OwnerCharacter.GetTemplate().Gender == Gender::Female)
		geosetVisualDataID = m_ItemDisplayInfoRecord->Get_HelmetGeosetFemaleID();
	else
		throw CException("CCharacterItem::CCharacterItem: Character has unkwnown gender '%d'.", m_OwnerCharacter.GetTemplate().Gender);

	if (geosetVisualDataID != 0)
	{
		m_HelmetGeosetVisDataRecord = m_BaseManager.GetManager<CDBCStorage>()->DBC_HelmetGeosetVisData()[geosetVisualDataID];
		if (m_HelmetGeosetVisDataRecord == nullptr)
			throw CException("CCharacterItem::CCharacterItem: DBC_HelmetGeosetVisData don't contains id '%d'.", geosetVisualDataID);
	}
}

CCharacterItem::~CCharacterItem()
{
}



bool CCharacterItem::IsNeedHideHelmetGeoset(EM2GeosetType M2GeosetType) const
{
	if (m_HelmetGeosetVisDataRecord == nullptr)
		return false;

	uint32 flags = 0;
	switch (M2GeosetType)
	{
		case EM2GeosetType::SkinAndHair:
			flags = m_HelmetGeosetVisDataRecord->Get_HairFlags();
		break;

		case EM2GeosetType::Facial01:
			flags = m_HelmetGeosetVisDataRecord->Get_Facial1Flags();
		break;

		case EM2GeosetType::Facial02:
			flags = m_HelmetGeosetVisDataRecord->Get_Facial2Flags();
		break;

		case EM2GeosetType::Facial03:
			flags = m_HelmetGeosetVisDataRecord->Get_Facial3Flags();
		break;

		case EM2GeosetType::Ears07:
			flags = m_HelmetGeosetVisDataRecord->Get_EarsFlags();
		break;

		default:
			return false;
	}

	return (flags & (1 << (uint8)m_OwnerCharacter.GetTemplate().Race)) != 0;
}



//
// ILoadable
//
bool CCharacterItem::Load()
{
	if (IsExists())
	{
		InitializeItemModels();
		InitializeItemGeosets();
		InitializeItemSkinImages();
	}

	return true;
}

bool CCharacterItem::Delete()
{
	return false;
}

void CCharacterItem::OnAfterLoad()
{
	m_OwnerCharacter.Refresh_SkinWithItemsImage();
	m_OwnerCharacter.RefreshMeshIDs();
}



//
// Private
//
void CCharacterItem::InitializeItemModels()
{
	for (uint32 i = 0; i < ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].AttachmentsCount; i++)
	{
		std::string objectM2ModelFileName = m_ItemDisplayInfoRecord->Get_ObjectModelName(i);
		std::string objectTextureName = m_ItemDisplayInfoRecord->Get_ObjectTextureName(i);

		if (GetTemplate().InventoryType == (uint8)DBCItem_EInventoryItemType::CLOAK)
		{
			auto cloackImage = LoadItemImage(DBCItem_EInventoryItemType::CLOAK, objectTextureName);
			m_Models.push_back({ nullptr, cloackImage });
			continue;
		}

		if (objectM2ModelFileName.empty())
			continue;

		// Create model and node for Item
		auto itemModel = LoadItemM2Model(static_cast<DBCItem_EInventoryItemType>(GetTemplate().InventoryType), objectM2ModelFileName);
		if (itemModel == nullptr)
			throw CException("CCharacterItem::InitializeItemModels: M2Model for item DisplayID '%d' not found.", GetTemplate().DisplayId);

		auto attachmentPoint = ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].AttachmentPoint[i];
		auto itemModelInstance = m_OwnerCharacter.CreateSceneNode<CCharacterItemM2Instance>(itemModel, *this, attachmentPoint);
		itemModelInstance->AddParentLoadable(std::dynamic_pointer_cast<ILoadable>(shared_from_this()));
		m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(itemModelInstance);

		if (auto ownerCharacterAttachment = m_OwnerCharacter.GetM2().getMiscellaneous().getAttachment(attachmentPoint))
			itemModelInstance->Attach(ownerCharacterAttachment->GetAttachmentType());

		auto itemImage = LoadItemImage((DBCItem_EInventoryItemType)GetTemplate().InventoryType, objectTextureName);
		itemModelInstance->SetSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, m_BaseManager.GetManager<IznTexturesFactory>()->LoadTexture2D(itemImage));

		m_Models.push_back({ itemModelInstance, itemImage });
	}
}

void CCharacterItem::InitializeItemGeosets()
{
	for (uint32 j = 0; j < MESHID_MAX_MODS; j++)
	{
		EM2GeosetType mesh = ItemObjectComponents[static_cast<size_t>(GetTemplate().InventoryType)].Geosets[j];
		if (mesh == EM2GeosetType::UNK)
			continue;

		m_Geosets.push_back({ mesh, m_ItemDisplayInfoRecord->Get_GeosetGroups(j) });
	}
}

void CCharacterItem::InitializeItemSkinImages()
{
	for (uint32 i = 0; i < static_cast<size_t>(DBC_CharComponent_Sections::ITEMS_COUNT); i++)
	{
		std::string textureComponentName = m_ItemDisplayInfoRecord->Get_TextureComponents(i);
		if (textureComponentName.empty())
			continue;

		m_SkinComponentImages[i] = LoadItemSkinComponentImage(ItemTextureComponents[i].list, textureComponentName);
	}
}

std::shared_ptr<CM2> CCharacterItem::LoadItemM2Model(DBCItem_EInventoryItemType InventoryItemType, std::string ModelFileName)
{
	std::string modelFilename = ModelFileName;

	// Head model FileName use gender and race in Filename
	if (InventoryItemType == DBCItem_EInventoryItemType::HEAD)
	{
		std::string raceClientPrefixString = m_BaseManager.GetManager<CDBCStorage>()->DBC_ChrRaces()[static_cast<size_t>(m_OwnerCharacter.GetTemplate().Race)]->Get_ClientPrefix();
		char genderLetter = GetGenderLetter(m_OwnerCharacter.GetTemplate().Gender);

		char modelPostfix[MAX_PATH];
		sprintf_s(modelPostfix, "_%s%c", raceClientPrefixString.c_str(), genderLetter);

		size_t dotPosition = modelFilename.find_last_of('.');
		if (dotPosition == std::string::npos)
			throw CException("ModelFileName must contains dot.");

		modelFilename.insert(dotPosition, modelPostfix);
	}

	std::string itemM2ModelFileName =  "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(InventoryItemType)].ModelsAndTexturesFolder) + "\\" + modelFilename;
	return m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, itemM2ModelFileName);
}

std::shared_ptr<IImage> CCharacterItem::LoadItemImage(DBCItem_EInventoryItemType InventoryItemType, std::string TextureFileName)
{
	std::string imageFilename = "Item\\ObjectComponents\\" + std::string(ItemObjectComponents[static_cast<size_t>(InventoryItemType)].ModelsAndTexturesFolder) + "\\" + TextureFileName + ".blp";
	return m_BaseManager.GetManager<IImagesFactory>()->CreateImage(imageFilename);
}

std::shared_ptr<IImage> CCharacterItem::LoadItemSkinComponentImage(DBC_CharComponent_Sections SkinComponent, std::string TextureFileName)
{
	std::string maleTexture = GetSkinComponentImageFileName(SkinComponent, TextureFileName, Gender::Male);
	std::string femaleTexture = GetSkinComponentImageFileName(SkinComponent, TextureFileName, Gender::Female);
	std::string universalTexture = GetSkinComponentImageFileName(SkinComponent, TextureFileName, Gender::None);

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
