#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWPlayer.h"

// Additional
#include "../World/ServerWorld.h"

namespace
{
	glm::vec2 OrientationToDirection(float Orientation)
	{
		glm::vec2 direction(0.0f);
		direction.x = glm::cos(glm::radians(Orientation));
		direction.y = -glm::sin(glm::radians(Orientation));
		direction = glm::normalize(direction);
		return direction;
	}
}

CowServerPlayer::CowServerPlayer(IScene& Scene, CowServerWorld& WoWWorld, CowGuid Guid)
	: CowServerUnit(Scene, WoWWorld, Guid)
{
	//m_ObjectType |= TYPEMASK_PLAYER;
	m_Values.SetValuesCount(PLAYER_END);
}

CowServerPlayer::~CowServerPlayer()
{
}

void CowServerPlayer::OnValuesUpdated(const UpdateMask & Mask)
{
	__super::OnValuesUpdated(Mask);

	/*if (Mask.GetBit(UNIT_FIELD_BYTES_0))
	{
		if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_DisplayID_ModelInstance))
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
		if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_DisplayID_ModelInstance))
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
		if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_DisplayID_ModelInstance))
		{
			characterModel->Template().facialStyle = GetFacialStyle();
		}
		else
			Log::Warn("PLAYER_BYTES_2 Error.");
	}
	
	if (Mask.GetBit(PLAYER_BYTES_3))
	{
		auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_DisplayID_ModelInstance);
		if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(m_DisplayID_ModelInstance))
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
			if (auto characterModel = std::dynamic_pointer_cast<CCharacter>(DisplayID_GetModelInstance()))
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


uint8 CowServerPlayer::GetSkinId() const 
{
	return m_Values.GetByteValue(PLAYER_BYTES, EPlayerBytes1Offsets::PLAYER_BYTES_OFFSET_SKIN_ID);
}

uint8 CowServerPlayer::GetFaceId() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES, EPlayerBytes1Offsets::PLAYER_BYTES_OFFSET_FACE_ID);
}

uint8 CowServerPlayer::GetHairStyleId() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES, EPlayerBytes1Offsets::PLAYER_BYTES_OFFSET_HAIR_STYLE_ID);
}

uint8 CowServerPlayer::GetHairColorId() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES, EPlayerBytes1Offsets::PLAYER_BYTES_OFFSET_HAIR_COLOR_ID);
}

uint8 CowServerPlayer::GetFacialStyle() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES_2, EPlayerBytes2Offsets::PLAYER_BYTES_2_OFFSET_FACIAL_STYLE);
}

uint8 CowServerPlayer::GetNativeGender() const 
{ 
	return m_Values.GetByteValue(PLAYER_BYTES_3, EPlayerBytes3Offsets::PLAYER_BYTES_3_OFFSET_GENDER);
}


//
// Protected
//
std::shared_ptr<CowServerPlayer> CowServerPlayer::Create(CowServerWorld& WoWWorld, IScene& Scene, CowGuid Guid)
{
	std::shared_ptr<CowServerPlayer> thisObj = MakeShared(CowServerPlayer, Scene, WoWWorld, Guid);
	return thisObj;
}

void CowServerPlayer::Destroy()
{
	__super::Destroy();
}



//
// Protected
//
void CowServerPlayer::OnDisplayIDChanged(uint32 DisplayID)
{
	if (DisplayID_GetModelInstance() != nullptr)
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
		DisplayID_SetModelInstance(creator.BuildCharacterFromTemplate(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), characterTemplate));
	}
	else
	{
		CWorldObjectCreator creator(GetScene().GetBaseManager());
		DisplayID_SetModelInstance(creator.BuildCreatureFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), DisplayID));
	}
}

void CowServerPlayer::Movement_HandlePlayerMovement(const UpdateEventArgs& e)
{
	if (Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_LEFT))
	{
		float speed = GetSpeed(EUnitSpeed::UNIT_SPEED_TURN_RATE) / 60.0f * e.DeltaTimeMultiplier;

		Orientation += glm::degrees(speed);
		CommitPositionAndRotation();
	}
	if (Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_RIGHT))
	{
		float speed = GetSpeed(EUnitSpeed::UNIT_SPEED_TURN_RATE) / 60.0f * e.DeltaTimeMultiplier;

		Orientation -= glm::degrees(speed);
		CommitPositionAndRotation();
	}
	if (Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_FORWARD))
	{
		float speed = GetSpeed(Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_WALKING) ? EUnitSpeed::UNIT_SPEED_WALK : EUnitSpeed::UNIT_SPEED_RUN) / 60.0f * e.DeltaTimeMultiplier;
		float forwardOrientation = Orientation;

		Position.xz += OrientationToDirection(forwardOrientation) * speed;
		CommitPositionAndRotation();
	}
	if (Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_BACKWARD))
	{
		float speed = GetSpeed(EUnitSpeed::UNIT_SPEED_RUN_BACK) / 60.0f * e.DeltaTimeMultiplier;
		float backwardOrientation = Orientation + glm::degrees(glm::pi<float>());

		Position.xz += OrientationToDirection(backwardOrientation) * speed;
		CommitPositionAndRotation();
	}
	if (Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_STRAFE_LEFT))
	{
		float speed = GetSpeed(Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_WALKING) ? EUnitSpeed::UNIT_SPEED_WALK : EUnitSpeed::UNIT_SPEED_RUN) / 60.0f * e.DeltaTimeMultiplier;
		float leftOrientation = Orientation + glm::degrees(glm::half_pi<float>());

		Position.xz += OrientationToDirection(leftOrientation) * speed;
		CommitPositionAndRotation();
	}
	if (Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_STRAFE_RIGHT))
	{
		float speed = GetSpeed(Movement_GetMovementInfo().HasMovementFlag(MOVEMENTFLAG_WALKING) ? EUnitSpeed::UNIT_SPEED_WALK : EUnitSpeed::UNIT_SPEED_RUN) / 60.0f * e.DeltaTimeMultiplier;
		float rightOrientation = Orientation - glm::degrees(glm::half_pi<float>());

		Position.xz += OrientationToDirection(rightOrientation) * speed;
		CommitPositionAndRotation();
	}
}



#endif
