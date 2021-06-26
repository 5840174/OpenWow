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
	//setMeshEnabled(MeshIDType::Ears, EarsStyles::Enabled);
	//setMeshEnabled(MeshIDType::Eyeglows, EyeglowsStyles::Racial);


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
	// 3. Items visual data
	for (uint32 i = 0; i < INVENTORY_SLOT_BAG_END; i++)
	{
		m_VisualItems[static_cast<EInventoryType>(i)]->TemplateSet(m_Template.ItemsTemplates[i]);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(m_VisualItems[static_cast<EInventoryType>(i)]);

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

		for (const auto& geoset : m_VisualItems[static_cast<EInventoryType>(i)]->getGeosetComponents())
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
	std::shared_ptr<const CItem_VisualData> item = m_VisualItems[static_cast<EInventoryType>(EQUIPMENT_SLOT_BACK)];
	if (item->m_InventoryType != (uint8)EInventoryType::NON_EQUIP)
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



//
// ISceneNode
//
void Character::Initialize()
{
	__super::Initialize();

	GetColliderComponent()->SetDebugDrawColor(ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	GetColliderComponent()->SetDebugDrawMode(true);

	for (uint32 slot = 0; slot < INVENTORY_SLOT_BAG_END; slot++) // TODO: Move to constuctor
		m_VisualItems[static_cast<EInventoryType>(slot)] = std::make_shared<CItem_VisualData>(getM2().GetBaseManager(), getM2().GetRenderDevice(), std::dynamic_pointer_cast<Character>(shared_from_this()));
}

#endif