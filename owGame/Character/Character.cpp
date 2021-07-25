#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "Character.h"

// Additional
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

CCharacter::CCharacter(IScene& Scene, CWorldClient& WorldClient, const std::shared_ptr<CM2>& M2Object, const SCharacterVisualTemplate& CharacterVisualTemplate)
	: CCreature(Scene, WorldClient, M2Object)
	, m_CharacterVisualTemplate(CharacterVisualTemplate)
	, m_IsNPCBakedTexturePresent(false)
{
}

CCharacter::~CCharacter()
{
	//Log::Warn("CCharacter deleted.");
}



//
// CCharacter
//
void CCharacter::SetNPCBakedImage(std::shared_ptr<IImage> BakedNPCImage)
{
	m_IsNPCBakedTexturePresent = true;
	m_BackedSkinImage = BakedNPCImage;
}

void CCharacter::Refresh_SkinImageFromTemplate()
{
	if (m_BackedSkinImage == nullptr)
	{
		if (m_IsNPCBakedTexturePresent)
			throw CException("Backed NPC skin iamge not exists, but m_IsNPCBakedTexturePresent is true.");

		m_BackedSkinImage = Character_SkinTextureBaker(GetBaseManager()).CreateCharacterSkinImage(GetTemplate());
	}

	Character_SectionWrapper sectionWrapper(GetBaseManager());

	SetSpecialTexture(SM2_Texture::Type::SKIN,       GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(m_BackedSkinImage));
	SetSpecialTexture(SM2_Texture::Type::SKIN_EXTRA, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(sectionWrapper.getSkinExtraTexture(GetTemplate())));
	SetSpecialTexture(SM2_Texture::Type::CHAR_HAIR,  GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(sectionWrapper.getHairTexture(GetTemplate())));
}

void CCharacter::Refresh_SkinWithItemsImage()
{
	if (GetState() != ILoadable::ELoadableState::Loaded)
		throw CException("Unexpected behaviour.");

	if (false == m_IsNPCBakedTexturePresent) // NCPBakedTexture already contains all items
	{
		if (m_BackedSkinImage == nullptr)
			throw CException("CCharacter::Refresh_SkinWithItemsImage: BakedSkinImage is nullptr.");

		auto skinImageWithItems = Character_SkinTextureBaker(GetBaseManager()).AddItemsTexturesToCharacterSkinImage(m_BackedSkinImage, this);
		SetSpecialTexture(SM2_Texture::Type::SKIN, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(skinImageWithItems));
	}

	// Special case for Cloack. Cloack isn't separate item model and clock texture is part of character.
	const auto& cloakItem = GetItem(EQUIPMENT_SLOT_BACK);
	if (cloakItem != nullptr && cloakItem->IsLoaded() && cloakItem->IsExists())
	{
		if (cloakItem->GetModels().size() != 1)
			throw CException("Character::Refresh_AddItemsToSkinTexture: Cape must contains one object component.");

		auto cloackImage = cloakItem->GetModels()[0].ItemSelfTexture;
		SetSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(cloackImage));
	}
}

void CCharacter::RefreshMeshIDs()
{
	// Refresh all geosets to default value
	for (uint32 i = 0; i < (size_t)EM2GeosetType::Count; i++)
		m_MeshID[i] = 1;

	// Initialize geosets from character template
	Character_SectionWrapper sectionWrapper(GetBaseManager());
	SetMeshEnabled(EM2GeosetType::SkinAndHair, sectionWrapper.getHairShowScalp(GetTemplate()) ? static_cast<uint32>(SkinAndHairStyles::ShowScalp) : sectionWrapper.getHairGeoset(GetTemplate()));
	SetMeshEnabled(EM2GeosetType::Facial01,    sectionWrapper.getFacial01Geoset(GetTemplate()));
	SetMeshEnabled(EM2GeosetType::Facial02,    sectionWrapper.getFacial02Geoset(GetTemplate()));
	SetMeshEnabled(EM2GeosetType::Facial03,    sectionWrapper.getFacial03Geoset(GetTemplate()));
	SetMeshEnabled(EM2GeosetType::Ears07,      (uint32)EarsStyles::Enabled);
	SetMeshEnabled(EM2GeosetType::Unk16,       sectionWrapper.getFacial16Geoset(GetTemplate()));
	SetMeshEnabled(EM2GeosetType::Eyeglows17,  sectionWrapper.getFacial17Geoset(GetTemplate()));

	// Change geoset from items
	for (size_t inventorySlot = 0; inventorySlot < INVENTORY_SLOT_BAG_END; inventorySlot++)
	{
		const auto& item = GetItem(inventorySlot);
		if (item != nullptr && item->IsLoaded() && item->IsExists())
			for (const auto& geoset : item->GetGeosets())
				SetMeshEnabled(geoset.mesh, geoset.getMeshID());
	}

	// Special case for Helmet. Hide some geosets on character head.
	const auto& helmetItem = GetItem(EQUIPMENT_SLOT_HEAD);
	if (helmetItem != nullptr && helmetItem->IsLoaded() && helmetItem->IsExists())
	{
		if (helmetItem->IsNeedHideHelmetGeoset(EM2GeosetType::SkinAndHair))
			SetMeshEnabled(EM2GeosetType::SkinAndHair, 1);

		if (helmetItem->IsNeedHideHelmetGeoset(EM2GeosetType::Facial01))
			SetMeshEnabled(EM2GeosetType::Facial01, 1);

		if (helmetItem->IsNeedHideHelmetGeoset(EM2GeosetType::Facial02))
			SetMeshEnabled(EM2GeosetType::Facial02, 1);

		if (helmetItem->IsNeedHideHelmetGeoset(EM2GeosetType::Facial03))
			SetMeshEnabled(EM2GeosetType::Facial03, 1);

		if (helmetItem->IsNeedHideHelmetGeoset(EM2GeosetType::Ears07))
			SetMeshEnabled(EM2GeosetType::Ears07, (uint32)EarsStyles::None);
	}
}



//
// Items
//
std::shared_ptr<CCharacterItem> CCharacter::GetItem(uint8 InventorySlot) const
{
	if (InventorySlot >= INVENTORY_SLOT_BAG_END)
		throw CException("CCharacter::GetItem: Incorrect inventory slot '%d'.", InventorySlot);

	return m_CharacterItems[InventorySlot];
}

void CCharacter::SetItem(uint8 InventorySlot, const SCharacterItemTemplate& ItemTemplate)
{
	if (InventorySlot >= INVENTORY_SLOT_BAG_END)
		throw CException("CCharacter::SetItem: Incorrect inventory slot '%d'.", InventorySlot);

	//if (m_CharacterItems[InventorySlot] != nullptr)
	//	GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_CharacterItems[InventorySlot]);

	// If item exists, then remove it from character
	auto itemInSlot = m_CharacterItems[InventorySlot];
	if (itemInSlot != nullptr && itemInSlot->IsExists())
	{
		for (const auto& itemObjectsIt : itemInSlot->GetModels())
		{
			if (itemObjectsIt.ItemM2Instance != nullptr)
			{
				const_cast<CCharacter*>(this)->RemoveChild(itemObjectsIt.ItemM2Instance);
			}
		}
	}

	auto characterItem = MakeShared(CCharacterItem, GetWorldClient(), GetBaseManager(), GetRenderDevice(), std::dynamic_pointer_cast<CCharacter>(shared_from_this()), ItemTemplate);
	AddChildLoadable(characterItem);
	m_CharacterItems[InventorySlot] = characterItem;
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(characterItem);
}



//
// Geosets
//
bool CCharacter::IsMeshEnabled(uint32 _index) const
{
	uint32 div100 = _index / 100;
	uint32 mod100 = _index % 100;

	_ASSERT(div100 < (uint32)EM2GeosetType::Count);
	_ASSERT(div100 != 6 && div100 != 14 && div100 != 16);

	for (uint32 i = 0; i < (uint32)EM2GeosetType::Count; i++)
	{
		// Special case for skin
		if (div100 == (uint32)EM2GeosetType::SkinAndHair && mod100 == 0)
			return true;

		// Others
		if (div100 == i)
			if (m_MeshID[i] == mod100)
				return true;
	}

	return false;
}
void CCharacter::SetMeshEnabled(EM2GeosetType M2GeosetType, uint32 _value)
{
	if (M2GeosetType >= EM2GeosetType::Count)
		throw CException("Character::SetMeshEnabled: GeosetType '%d' out of bounds.", (uint32)M2GeosetType);

	if (_value == UINT32_MAX)
		return;

	m_MeshID[(size_t)M2GeosetType] = _value;
}



//
// ISceneNode
//
void CCharacter::Initialize()
{
	__super::Initialize();

	SetDebugDrawColor(ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	SetDebugDrawMode(true);
}



bool CCharacter::Load()
{
	if (false == __super::Load())
		return false;

	Refresh_SkinImageFromTemplate();
	RefreshMeshIDs();

	return true;
}

//
// ILoadable
//
void CCharacter::OnAfterLoad()
{
	if (GetM2().GetState() != ILoadable::ELoadableState::Loaded)
		throw CException("Unexpected behaviour.");

	//Refresh_SkinImageFromTemplate();
	//RefreshMeshIDs();
	//RefreshMeshIDs();
}

#endif