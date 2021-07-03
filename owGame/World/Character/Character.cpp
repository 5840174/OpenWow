#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "Character.h"

// Additional
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

Character::Character(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: Creature(Scene, M2Object)
	, m_IsNPCBakedTexturePresent(false)
{
	for (uint32 i = 0; i < (size_t)MeshIDType::Count; i++)
		m_MeshID[i] = 1;

	setMeshEnabled(MeshIDType::Ears07, (uint32)EarsStyles::Enabled);
	setMeshEnabled(MeshIDType::Eyeglows17, (uint32)EyeglowsStyles::Racial);
}

Character::~Character()
{}

void Character::RefreshItemVisualData()
{
	for (uint32 i = 0; i < INVENTORY_SLOT_BAG_END; i++)
	{
		DBCItem_EInventoryItemSlot itemInventoryType = static_cast<DBCItem_EInventoryItemSlot>(i);
		const auto& itemVisualData = m_VisualItems[itemInventoryType];
		
		itemVisualData->Template() = m_Template.ItemsTemplates[i];
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(itemVisualData);
	}
}

void Character::Refresh_CreateSkinTexture(std::shared_ptr<IImage> BakedSkinImage)
{
	if (m_BackedSkinImage == nullptr)
	{
		if (BakedSkinImage != nullptr)
		{
			m_IsNPCBakedTexturePresent = true;
			m_BackedSkinImage = BakedSkinImage;
		}
		else
		{
			m_BackedSkinImage = Character_SkinTextureBaker(GetBaseManager()).CreateCharacterSkinImage(GetTemplate());
		}
	}

	Character_SectionWrapper sectionWrapper(GetBaseManager());

	setSpecialTexture(SM2_Texture::Type::SKIN,       GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(m_BackedSkinImage));
	setSpecialTexture(SM2_Texture::Type::SKIN_EXTRA, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(sectionWrapper.getSkinExtraTexture(GetTemplate())));
	setSpecialTexture(SM2_Texture::Type::CHAR_HAIR,  GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(sectionWrapper.getHairTexture(GetTemplate())));
}

void Character::Refresh_AddItemsToSkinTexture()
{
	if (false == m_IsNPCBakedTexturePresent) // NCPBakedTexture already contains all items
	{
		auto skinImageWithItems = Character_SkinTextureBaker(GetBaseManager()).CreateCharacterSkinWithItemsImage(m_BackedSkinImage, this);
		setSpecialTexture(SM2_Texture::Type::SKIN, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(skinImageWithItems));
	}

	// Cloak is special texture
	auto& cloakVisualItem = m_VisualItems[static_cast<DBCItem_EInventoryItemSlot>(EQUIPMENT_SLOT_BACK)];
	if (cloakVisualItem->GetTemplate().InventoryType != (uint8)DBCItem_EInventoryItemSlot::NON_EQUIP)
	{
		if (cloakVisualItem->getObjectComponents().size() != 1)
			return;
		auto cloackImage = cloakVisualItem->getObjectComponents()[0].texture;
		setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(cloackImage));
	}
}

void Character::RefreshMeshIDs()
{
	Character_SectionWrapper sectionWrapper(GetBaseManager());

	if (sectionWrapper.getHairShowScalp(GetTemplate()))
		setMeshEnabled(MeshIDType::SkinAndHair, 1);
	else
		setMeshEnabled(MeshIDType::SkinAndHair, sectionWrapper.getHairGeoset(GetTemplate()));

	setMeshEnabled(MeshIDType::Facial01,   sectionWrapper.getFacial01Geoset(GetTemplate()));
	setMeshEnabled(MeshIDType::Facial02,   sectionWrapper.getFacial02Geoset(GetTemplate()));
	setMeshEnabled(MeshIDType::Facial03,   sectionWrapper.getFacial03Geoset(GetTemplate()));
	setMeshEnabled(MeshIDType::Unk16,      sectionWrapper.getFacial16Geoset(GetTemplate()));
	setMeshEnabled(MeshIDType::Eyeglows17, sectionWrapper.getFacial17Geoset(GetTemplate()));


	for (uint32 i = 0; i < INVENTORY_SLOT_BAG_END; i++)
	{
		DBCItem_EInventoryItemSlot itemInventoryType = static_cast<DBCItem_EInventoryItemSlot>(i);
		const auto& itemVisualData = m_VisualItems[itemInventoryType];

		if (i == EQUIPMENT_SLOT_HEAD)
		{
			if ((m_Template.Flags & CHARACTER_FLAG_HIDE_HELM))
			{
				setMeshEnabled(MeshIDType::Ears07, (uint32)EarsStyles::Enabled);
				continue;
			}
			else
			{
				setMeshEnabled(MeshIDType::Ears07, (uint32)EarsStyles::None);
			}
		}
		else if (i == EQUIPMENT_SLOT_BACK)
		{
			if ((m_Template.Flags & CHARACTER_FLAG_HIDE_CLOAK))
			{
				setMeshEnabled(MeshIDType::Cloak15, 1);
				continue;
			}
			else
			{

			}
		}

		for (const auto& geoset : itemVisualData->getGeosetComponents())
		{
			setMeshEnabled(geoset.mesh, geoset.getMeshID());
		}
	}
}



void Character::setMeshEnabled(MeshIDType _type, uint32 _value)
{
	if (_type >= MeshIDType::Count)
		throw CException("Character::setMeshEnabledL: MeshType '%d' out of bounds.", (uint32)_type);

	if (_value == UINT32_MAX)
		return;

	m_MeshID[(size_t)_type] = _value;
}

bool Character::isMeshEnabled(uint32 _index) const
{
	uint32 div100 = _index / 100;
	uint32 mod100 = _index % 100;

	//_ASSERT(div100 < MeshIDType::Count);
	_ASSERT(div100 != 6);
	_ASSERT(div100 != 14);
	_ASSERT(div100 != 16);

	//if (div100 == 3)
	//	return true;

	for (uint32 i = 0; i < (uint32)MeshIDType::Count; i++)
	{
		// Special case for skin
		if (div100 == (uint32)MeshIDType::SkinAndHair && mod100 == 0)
		{
			return true;
		}

		// Others
		if (div100 == i)
		{
			if (m_MeshID[i] == mod100)
			{
				return true;
			}
		}
	}

	return false;
}



//
// ISceneNode
//
void Character::Initialize()
{
	__super::Initialize();

	GetColliderComponent()->SetDebugDrawColor(ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	GetColliderComponent()->SetDebugDrawMode(true);

	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++) // TODO: Move to constuctor
		m_VisualItems[static_cast<DBCItem_EInventoryItemSlot>(slot)] = MakeShared(CCharacterVisualItem, getM2().GetBaseManager(), getM2().GetRenderDevice(), std::dynamic_pointer_cast<Character>(shared_from_this()));
}



//
// ILoadable
//
bool Character::Load()
{
	return __super::Load();
}

#endif