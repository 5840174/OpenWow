#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "Character.h"

// Additional
#include "Character_SectionWrapper.h"
#include "Character_SkinTextureBaker.h"

Character::Character(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: Creature(Scene, M2Object)
{
	for (uint32 i = 0; i < (size_t)MeshIDType::Count; i++)
		m_MeshID[i] = 1;

	setMeshEnabled(MeshIDType::Ears, (uint32)EarsStyles::Enabled);
	setMeshEnabled(MeshIDType::Eyeglows, (uint32)EyeglowsStyles::Racial);
}

Character::~Character()
{
}


	//CM2_Base_Instance::Render3D();

	/*for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++)
	{
		if (slot == EQUIPMENT_SLOT_HEAD && (m_Template.Flags & CHARACTER_FLAG_HIDE_HELM))
		{
			continue;
		}

		m_VisualItems[slot]->Render3D();
	}*/





void Character::RefreshItemVisualData()
{
	for (uint32 i = 0; i < INVENTORY_SLOT_BAG_END; i++)
	{
		DBCItem_EInventoryType itemInventoryType = static_cast<DBCItem_EInventoryType>(i);
		std::shared_ptr<CItem_VisualData> itemVisualData = m_VisualItems[itemInventoryType];
		
		itemVisualData->TemplateSet(m_Template.ItemsTemplates[i]);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(itemVisualData);

		if (i == EQUIPMENT_SLOT_HEAD)
		{
			if ((m_Template.Flags & CHARACTER_FLAG_HIDE_HELM))
			{
				setMeshEnabled(MeshIDType::Ears, (uint32)EarsStyles::Enabled);
				continue;
			}
			else
			{
				setMeshEnabled(MeshIDType::Ears, (uint32)EarsStyles::None);
			}
		}
		else if (i == EQUIPMENT_SLOT_BACK)
		{
			if ((m_Template.Flags & CHARACTER_FLAG_HIDE_CLOAK))
			{
				setMeshEnabled(MeshIDType::Cloak, 1);
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

void Character::RefreshTextures(const Character_SectionWrapper& SectionWrapper, std::shared_ptr<ITexture> SkinTexture)
{
	setSpecialTexture(SM2_Texture::Type::SKIN, SkinTexture);
	setSpecialTexture(SM2_Texture::Type::SKIN_EXTRA, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(SectionWrapper.getSkinExtraTexture(GetTemplate())));
	setSpecialTexture(SM2_Texture::Type::CHAR_HAIR, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(SectionWrapper.getHairTexture(GetTemplate())));

	// Cloak
	std::shared_ptr<const CItem_VisualData> item = m_VisualItems[static_cast<DBCItem_EInventoryType>(EQUIPMENT_SLOT_BACK)];
	if (item->m_InventoryType != (uint8)DBCItem_EInventoryType::NON_EQUIP)
	{
		_ASSERT(item->getObjectComponents().size() == 1);
		auto cloackImage = item->getObjectComponents()[0].texture;
		setSpecialTexture(SM2_Texture::Type::OBJECT_SKIN, GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(cloackImage));
	}
}

void Character::RefreshMeshIDs(const Character_SectionWrapper& SectionWrapper)
{
	if (SectionWrapper.getHairShowScalp(GetTemplate()))
		setMeshEnabled(MeshIDType::SkinAndHair, 1);
	else
		setMeshEnabled(MeshIDType::SkinAndHair, SectionWrapper.getHairGeoset(GetTemplate()));

	setMeshEnabled(MeshIDType::Facial1,  SectionWrapper.getFacial01Geoset(GetTemplate()));
	setMeshEnabled(MeshIDType::Facial2,  SectionWrapper.getFacial02Geoset(GetTemplate()));
	setMeshEnabled(MeshIDType::Facial3,  SectionWrapper.getFacial03Geoset(GetTemplate()));
	setMeshEnabled(MeshIDType::Unk16,    SectionWrapper.getFacial16Geoset(GetTemplate()));
	setMeshEnabled(MeshIDType::Eyeglows, SectionWrapper.getFacial17Geoset(GetTemplate()));
}



void Character::setMeshEnabled(MeshIDType _type, uint32 _value)
{
	_ASSERT(_type < MeshIDType::Count);
	if (_value == UINT32_MAX)
	{
		return;
	}

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
		m_VisualItems[static_cast<DBCItem_EInventoryType>(slot)] = std::make_shared<CItem_VisualData>(getM2().GetBaseManager(), getM2().GetRenderDevice(), std::dynamic_pointer_cast<Character>(shared_from_this()));
}

#endif