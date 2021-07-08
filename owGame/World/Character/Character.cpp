#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "Character.h"

// Additional
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

CCharacter::CCharacter(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: CCreature(Scene, M2Object)
	, m_IsNPCBakedTexturePresent(false)
{
	for (uint32 i = 0; i < (size_t)EM2GeosetType::Count; i++)
		m_MeshID[i] = 1;

	setMeshEnabled(EM2GeosetType::Ears07, (uint32)EarsStyles::Enabled);
	setMeshEnabled(EM2GeosetType::Eyeglows17, (uint32)EyeglowsStyles::Racial);
}

CCharacter::~CCharacter()
{
	Log::Warn("Characted deleted.");
}



//
// CCharacter
//
void CCharacter::SetNPCBakedImage(std::shared_ptr<IImage> BakedNPCImage)
{
	m_IsNPCBakedTexturePresent = true;
	m_BackedSkinImage = BakedNPCImage;
}

void CCharacter::Refresh_CharacterItemsFromTemplate()
{
	for (uint32 inventorySlot = 0; inventorySlot < INVENTORY_SLOT_BAG_END; inventorySlot++)
		SetItem(inventorySlot, m_Template.ItemsTemplates[inventorySlot]);
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

	setSpecialTexture(SM2_Texture::Type::SKIN,       GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(m_BackedSkinImage));
	setSpecialTexture(SM2_Texture::Type::SKIN_EXTRA, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(sectionWrapper.getSkinExtraTexture(GetTemplate())));
	setSpecialTexture(SM2_Texture::Type::CHAR_HAIR,  GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(sectionWrapper.getHairTexture(GetTemplate())));
}

void CCharacter::Refresh_SkinWithItemsImage()
{
	if (false == m_IsNPCBakedTexturePresent) // NCPBakedTexture already contains all items
	{
		if (m_BackedSkinImage == nullptr)
			throw CException("Unable to refresh items.");

		auto skinImageWithItems = Character_SkinTextureBaker(GetBaseManager()).CreateCharacterSkinWithItemsImage(m_BackedSkinImage, this);
		setSpecialTexture(SM2_Texture::Type::SKIN, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(skinImageWithItems));
	}

	// Cloak is special texture
	const auto& cloakVisualItem = GetItem(EQUIPMENT_SLOT_BACK);
	if (cloakVisualItem != nullptr && cloakVisualItem->GetState() == ILoadable::ELoadableState::Loaded && cloakVisualItem->IsExists())
	{
		if (cloakVisualItem->GetModels().size() != 1)
			throw CException("Character::Refresh_AddItemsToSkinTexture: Cape must contains one object component.");
		auto cloackImage = cloakVisualItem->GetModels()[0].ItemSelfTexture;
		setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(cloackImage));
	}
}

void CCharacter::RefreshMeshIDs()
{
	// From template
	Character_SectionWrapper sectionWrapper(GetBaseManager());
	setMeshEnabled(EM2GeosetType::SkinAndHair, sectionWrapper.getHairShowScalp(GetTemplate()) ? static_cast<uint32>(SkinAndHairStyles::ShowScalp) : sectionWrapper.getHairGeoset(GetTemplate()));
	setMeshEnabled(EM2GeosetType::Facial01,    sectionWrapper.getFacial01Geoset(GetTemplate()));
	setMeshEnabled(EM2GeosetType::Facial02,    sectionWrapper.getFacial02Geoset(GetTemplate()));
	setMeshEnabled(EM2GeosetType::Facial03,    sectionWrapper.getFacial03Geoset(GetTemplate()));
	setMeshEnabled(EM2GeosetType::Unk16,       sectionWrapper.getFacial16Geoset(GetTemplate()));
	setMeshEnabled(EM2GeosetType::Eyeglows17,  sectionWrapper.getFacial17Geoset(GetTemplate()));

	// From items
	for (size_t inventorySlot = 0; inventorySlot < INVENTORY_SLOT_BAG_END; inventorySlot++)
	{
		const auto& characterItem = GetItem(inventorySlot);
		if (characterItem == nullptr || characterItem->GetState() != ILoadable::ELoadableState::Loaded || false == characterItem->IsExists())
			continue;

		if (inventorySlot == EQUIPMENT_SLOT_HEAD)
		{
			if (m_Template.IsHasCharacterFlag(CHARACTER_FLAG_HIDE_HELM))
			{
				setMeshEnabled(EM2GeosetType::Ears07, (uint32)EarsStyles::Enabled);
				continue;
			}
			else
			{
				setMeshEnabled(EM2GeosetType::Ears07, (uint32)EarsStyles::None);
			}
		}
		else if (inventorySlot == EQUIPMENT_SLOT_BACK)
		{
			if (m_Template.IsHasCharacterFlag(CHARACTER_FLAG_HIDE_CLOAK))
			{
				setMeshEnabled(EM2GeosetType::Cloak15, 1);
				continue;
			}
			else
			{

			}
		}

		for (const auto& geoset : characterItem->GetGeosets())
		{
			setMeshEnabled(geoset.mesh, geoset.getMeshID());
		}
	}
}



//
// Items
//
std::shared_ptr<CCharacterItem> CCharacter::GetItem(uint8 InventorySlot) const
{
	if (InventorySlot >= INVENTORY_SLOT_BAG_END)
		throw CException("CCharacter::GetItem: Incorrect inventory slot '%d'.", InventorySlot);

	//if (m_CharacterItems[InventorySlot] && m_CharacterItems[InventorySlot]->GetState() != ILoadable::ELoadableState::Loaded)
	//	throw CException("Item isn't exists.");

	return m_CharacterItems[InventorySlot];
}

void CCharacter::SetItem(uint8 InventorySlot, const SCharacterItemTemplate & ItemTemplate)
{
	if (InventorySlot >= INVENTORY_SLOT_BAG_END)
		throw CException("CCharacter::SetItem: Incorrect inventory slot '%d'.", InventorySlot);

	if (m_CharacterItems[InventorySlot] != nullptr)
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_CharacterItems[InventorySlot]);

	auto characterItem = MakeShared(CCharacterItem, GetBaseManager(), GetRenderDevice(), std::dynamic_pointer_cast<CCharacter>(shared_from_this()));
	Template().ItemsTemplates[InventorySlot] = ItemTemplate;
	characterItem->Template() = ItemTemplate;
	m_CharacterItems[InventorySlot] = characterItem;

	if (GetState() == ILoadable::ELoadableState::Loaded)
	{
		AddChildLoadable(characterItem);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(characterItem);
	}
}



//
// Geosets
//
void CCharacter::setMeshEnabled(EM2GeosetType M2GeosetType, uint32 _value)
{
	if (M2GeosetType >= EM2GeosetType::Count)
		throw CException("Character::setMeshEnabled: GeosetType '%d' out of bounds.", (uint32)M2GeosetType);

	if (_value == UINT32_MAX)
		return;

	m_MeshID[(size_t)M2GeosetType] = _value;
}

bool CCharacter::isMeshEnabled(uint32 _index) const
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



//
// ISceneNode
//
void CCharacter::Initialize()
{
	__super::Initialize();

	GetColliderComponent()->SetDebugDrawColor(ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	GetColliderComponent()->SetDebugDrawMode(true);
}



//
// ILoadable
//
void CCharacter::OnLoaded()
{
	if (getM2().GetState() != ILoadable::ELoadableState::Loaded)
		throw CException("Unexpected behaviour.");

	Refresh_SkinImageFromTemplate();
	Refresh_CharacterItemsFromTemplate();

	RefreshMeshIDs();

	for (size_t inventorySlot = 0; inventorySlot < INVENTORY_SLOT_BAG_END; inventorySlot++)
	{
		const auto& characterItem = GetItem(inventorySlot);
		if (characterItem && characterItem->GetState() < ILoadable::ELoadableState::Loaded && characterItem->IsExists())
		{
			AddChildLoadable(characterItem);
			GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(characterItem);
		}
	}
}

#endif