#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWPlayer.h"

// Additional
#include "World.h"
#include "World/WorldObjectsCreator.h"




WoWPlayer::WoWPlayer(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid)
	: WoWUnit(Scene, WoWWorld, Guid)
{
	m_ObjectType |= TYPEMASK_PLAYER;
	m_Values.SetValuesCount(PLAYER_END);
}

WoWPlayer::~WoWPlayer()
{
}

void WoWPlayer::OnValuesUpdated(const UpdateMask & Mask)
{
	if (Mask.GetBit(UNIT_FIELD_DISPLAYID))
	{
		if (m_HiddenNode != nullptr)
		{
			//Log::Warn("WoWUnit: UNIT_FIELD_DISPLAYID updated, but Node already exists.");
			return;
		}

		CWorldObjectCreator creator(GetScene().GetBaseManager());
		m_HiddenNode = creator.BuildEmptyCharacterFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), m_Values.GetUInt32Value(UNIT_FIELD_DISPLAYID));
	

		if (Mask.GetBit(UNIT_FIELD_BYTES_0))
		{
			auto characterModel = std::dynamic_pointer_cast<Character>(m_HiddenNode);
			characterModel->GetTemplate().Race = (Race)GetRace();
			characterModel->GetTemplate().Class = (Class)GetClass();
			characterModel->GetTemplate().Gender = (Gender)GetGender();
		}

		if (Mask.GetBit(PLAYER_BYTES))
		{
			auto characterModel = std::dynamic_pointer_cast<Character>(m_HiddenNode);

			characterModel->GetTemplate().skin = GetSkinId();
			characterModel->GetTemplate().face = GetFaceId();
			characterModel->GetTemplate().hairStyle = GetHairStyleId();
			characterModel->GetTemplate().hairColor = GetHairColorId();
		}

		if (Mask.GetBit(PLAYER_BYTES_2))
		{
			auto characterModel = std::dynamic_pointer_cast<Character>(m_HiddenNode);
			characterModel->GetTemplate().facialStyle = GetFacialStyle();
		}
	
		if (Mask.GetBit(PLAYER_BYTES_3))
		{
			auto characterModel = std::dynamic_pointer_cast<Character>(m_HiddenNode);
			characterModel->GetTemplate().Gender = (Gender)GetNativeGender();
		}

		for (uint16 i = PLAYER_VISIBLE_ITEM_1_ENTRYID; i < PLAYER_VISIBLE_ITEM_19_ENTRYID; i += 2)
		{
			if (Mask.GetBit(i))
			{
				auto characterModel = std::dynamic_pointer_cast<Character>(m_HiddenNode);
				characterModel->GetTemplate().ItemsTemplates[(i - PLAYER_VISIBLE_ITEM_1_ENTRYID) / 2] = GetItemDisplayInfoIDByItemID(m_Values.GetUInt32Value(i));
			}
		} 

		if (auto characterModel = std::dynamic_pointer_cast<Character>(m_HiddenNode))
		{
			Character_SectionWrapper sectionWrapper(GetBaseManager());

			characterModel->RefreshItemVisualData();

			Character_SkinTextureBaker skinTextureBaker(GetBaseManager(), GetBaseManager().GetApplication().GetRenderDevice());
			auto skinTexture = skinTextureBaker.createTexture(characterModel.get());
			characterModel->RefreshTextures(sectionWrapper, skinTexture);

			characterModel->RefreshMeshIDs(sectionWrapper);
		}


		
	}

	//__super::OnValuesUpdated(Mask);
}



//
// Protected
//
std::shared_ptr<WoWPlayer> WoWPlayer::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWPlayer> thisObj = MakeShared(WoWPlayer, Scene, WoWWorld, Guid);
	return thisObj;
}

void WoWPlayer::AfterCreate(IScene & Scene)
{
	__super::AfterCreate(Scene);
}


void WoWPlayer::Destroy()
{}

#endif
