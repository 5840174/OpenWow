#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWPlayer.h"

// Additional
#include "../World/World.h"

WoWPlayer::WoWPlayer(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid)
	: WoWUnit(Scene, WoWWorld, Guid)
{
	//m_ObjectType |= TYPEMASK_PLAYER;
	m_Values.SetValuesCount(PLAYER_END);
}

WoWPlayer::~WoWPlayer()
{
}

void WoWPlayer::OnValuesUpdated(const UpdateMask & Mask)
{
	__super::OnValuesUpdated(Mask);

	/*if (Mask.GetBit(UNIT_FIELD_BYTES_0))
	{
		if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_UnitModel))
		{
			characterModel->Template().Race = GetRace();
			characterModel->Template().Class = GetClass();
			characterModel->Template().Gender = GetGender();
		}
		else
			Log::Warn("UNIT_FIELD_BYTES_0 Error.");
	}*/

	/*if (Mask.GetBit(PLAYER_BYTES))
	{
		if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_UnitModel))
		{
			characterModel->Template().skin = GetSkinId();
			characterModel->Template().face = GetFaceId();
			characterModel->Template().hairStyle = GetHairStyleId();
			characterModel->Template().hairColor = GetHairColorId();
		}
		else
			Log::Warn("PLAYER_BYTES Error.");
	}

	if (Mask.GetBit(PLAYER_BYTES_2))
	{
		if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_UnitModel))
		{
			characterModel->Template().facialStyle = GetFacialStyle();
		}
		else
			Log::Warn("PLAYER_BYTES_2 Error.");
	}
	
	if (Mask.GetBit(PLAYER_BYTES_3))
	{
		auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_UnitModel);
		if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_UnitModel))
		{
			characterModel->Template().Gender = (Gender)GetNativeGender();
		}
		else
			Log::Warn("PLAYER_BYTES_3 Error.");
	}*/

	for (uint16 i = PLAYER_VISIBLE_ITEM_1_ENTRYID; i < PLAYER_VISIBLE_ITEM_19_ENTRYID; i += 2)
	{
		if (Mask.GetBit(i))
		{
			if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_UnitModel))
			{
				uint8 inventorySlot = (i - PLAYER_VISIBLE_ITEM_1_ENTRYID) / 2;
				if (inventorySlot >= EQUIPMENT_SLOT_END)
					throw CException("Inventory slot out of bounds.");

				uint32 displayID = m_Values.GetUInt32Value(i);
				uint32 enchantID = 0;
				if (Mask.GetBit(i + 1))
					enchantID = m_Values.GetUInt32Value(i + 1);
				characterModel->SetItem(inventorySlot, GetItemDisplayInfoIDByItemID(displayID, enchantID));
			}
			//else
			//	Log::Warn("PLAYER_VISIBLE_ITEM_%d_ENTRYID Error.", i);
		}
	}
}


uint8 WoWPlayer::GetSkinId() const 
{
	return m_Values.GetByteValue(PLAYER_BYTES, EPlayerBytes1Offsets::PLAYER_BYTES_OFFSET_SKIN_ID);
}

uint8 WoWPlayer::GetFaceId() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES, EPlayerBytes1Offsets::PLAYER_BYTES_OFFSET_FACE_ID);
}

uint8 WoWPlayer::GetHairStyleId() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES, EPlayerBytes1Offsets::PLAYER_BYTES_OFFSET_HAIR_STYLE_ID);
}

uint8 WoWPlayer::GetHairColorId() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES, EPlayerBytes1Offsets::PLAYER_BYTES_OFFSET_HAIR_COLOR_ID);
}

uint8 WoWPlayer::GetFacialStyle() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES_2, EPlayerBytes2Offsets::PLAYER_BYTES_2_OFFSET_FACIAL_STYLE);
}

uint8 WoWPlayer::GetNativeGender() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES_3, EPlayerBytes3Offsets::PLAYER_BYTES_3_OFFSET_GENDER);
}


//
// Protected
//
std::shared_ptr<WoWPlayer> WoWPlayer::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid)
{
	std::shared_ptr<WoWPlayer> thisObj = MakeShared(WoWPlayer, Scene, WoWWorld, Guid);
	return thisObj;
}

void WoWPlayer::Destroy()
{}



//
// Protected
//
void WoWPlayer::OnDisplayIDChanged(uint32 DisplayID)
{
	if (m_UnitModel != nullptr)
		return;

	if (false == m_Values.IsExists(UNIT_FIELD_NATIVEDISPLAYID))
		throw CException("UNIT_FIELD_NATIVEDISPLAYID must exists.");

	if (DisplayID == m_Values.GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID))
	{
		SCharacterTemplate characterTemplate;
		characterTemplate.Race = GetRace();
		characterTemplate.Gender = GetGender();
		characterTemplate.skin = GetSkinId();
		characterTemplate.face = GetFaceId();
		characterTemplate.hairStyle = GetHairStyleId();
		characterTemplate.hairColor = GetHairColorId();
		characterTemplate.facialStyle = GetFacialStyle();

		CWorldObjectCreator creator(GetScene().GetBaseManager());
		m_UnitModel = creator.BuildCharacterFromTemplate(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), characterTemplate);
	}
	else
	{
		CWorldObjectCreator creator(GetScene().GetBaseManager());
		m_UnitModel = creator.BuildCreatureFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), DisplayID);
	}

	float scale = m_Values.GetFloatValue(OBJECT_FIELD_SCALE_X);
	m_UnitModel->SetScale(glm::vec3(scale));

	if (m_MountModel)
		m_MountModel->AddChild(m_UnitModel);
}



#endif
