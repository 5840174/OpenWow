#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWUnit.h"

// Additional
#include "../../World/ServerWorld.h"


float gravity = static_cast<float>(19.29110527038574);



CowServerUnit::CowServerUnit(IScene& Scene, CowServerWorld& WoWWorld, CowGuid Guid)
	: CowServerWorldObject(Scene, WoWWorld, Guid)
	
	, m_DisplayID_ID(0)
	, m_DisplayID_Scale_IsDirty(false)
	, m_DisplayID_Scale(1.0f)
	
	, m_Movement_IsMoveNow(false)

	, m_Jump_IsJumpingNow(false)
	, m_Mount_IsMounted(false)
	, m_Mount_IsDirty(false)
{
	//m_ObjectType |= TYPEMASK_UNIT;
	m_Values.SetValuesCount(UNIT_END);
}

CowServerUnit::~CowServerUnit()
{
	//Destroy();
}

void CowServerUnit::ProcessMovementPacket(CServerPacket & Bytes)
{
	ReadMovementInfoPacket(Bytes);

	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_WALK];
	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_RUN];
	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_RUN_BACK];
	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_SWIM];
	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_SWIM_BACK];
	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_FLIGHT];
	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_FLIGHT_BACK];
	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_TURN_RATE];
	Bytes >> m_Movement_Speed[EUnitSpeed::UNIT_SPEED_PITCH_RATE];

	if (m_MovementInfo.HasMovementFlag(MOVEMENTFLAG_SPLINE_ENABLED))
	{
		auto path = MakeShared(CWoWPath);

		Movement::MoveSplineFlag splineFlags;
		Bytes >> splineFlags;

		if (splineFlags.final_angle)
		{
			float angle;
			Bytes >> angle;
		}
		else if (splineFlags.final_target)
		{
			uint64 targetGUID;
			Bytes >> targetGUID;
		}
		else if (splineFlags.final_point)
		{
			glm::vec3 targetPoint;
			Bytes >> targetPoint;
		}

		int32 timePassed, duration;
		Bytes >> timePassed;
		Bytes >> duration;

		path->SetDuration(duration);
		path->SetCurrTime(timePassed);

		uint32 splineID;
		Bytes >> splineID;

		Bytes.seekRelative(4); // splineInfo.duration_mod; added in 3.1      ALWAYS 1.0f
		Bytes.seekRelative(4); // splineInfo.duration_mod_next; added in 3.1 ALWAYS 1.0f

		Bytes.seekRelative(4); // move_spline.vertical_acceleration;
		Bytes.seekRelative(4); // move_spline.effect_start_time;

		uint32 pathNodesCount;
		Bytes >> pathNodesCount;
		for (size_t i = 0; i < pathNodesCount; i++)
		{
			glm::vec3 pathNodePoint;
			Bytes >> pathNodePoint;
			path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(pathNodePoint)));
		}

		enum EEvaluationMode : uint8
		{
			ModeLinear,
			ModeCatmullrom,
			ModeBezier3_Unused,
			UninitializedMode,
			ModesEnd
		} splineMode;
		Bytes >> splineMode;

		if (splineFlags.cyclic)
		{
			Bytes.seekRelative(12); // Zero vector
		}
		else
		{
			glm::vec3 finalDestination;
			Bytes >> finalDestination;
			path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(finalDestination)));
		}

		m_WoWPath = path;
	}

	CommitPositionAndRotation();
}

void CowServerUnit::Do_MonsterMove(CServerPacket& Bytes)
{
	glm::vec3 firstSplinePointGame;
	Bytes >> firstSplinePointGame;
	Position = fromGameToReal(firstSplinePointGame);

	uint32 splineID;
	Bytes >> splineID;

	enum EMonsterMoveType : uint8
	{
		MonsterMoveNormal = 0,
		MonsterMoveStop = 1,
		MonsterMoveFacingSpot = 2,
		MonsterMoveFacingTarget = 3,
		MonsterMoveFacingAngle = 4
	} monsterMoveType;
	Bytes >> monsterMoveType;

	switch (monsterMoveType)
	{
		case EMonsterMoveType::MonsterMoveFacingSpot:
		{
			glm::vec3 targetPoint;
			Bytes >> targetPoint;
		}
		break;

		case EMonsterMoveType::MonsterMoveFacingTarget:
		{
			uint64 targetGUID;
			Bytes >> targetGUID;
		}
		break;

		case EMonsterMoveType::MonsterMoveFacingAngle:
		{
			float angle;
			Bytes >> angle;
		}
		break;

		case EMonsterMoveType::MonsterMoveNormal:
		{
			// normal packet
		}
		break;

		case EMonsterMoveType::MonsterMoveStop:
		{
			CommitPositionAndRotation();
			return;
		}
	}
	
	auto path = MakeShared(CWoWPath);

	Movement::MoveSplineFlag splineflags;
	Bytes >> splineflags;

	if (splineflags.animation)
	{
		uint8 animationID;
		Bytes >> animationID;

		int32 effectStartTime;
		Bytes >> effectStartTime;
	}

	int32 duration;
	Bytes >> duration;
	path->SetDuration(duration);

	if (splineflags.parabolic)
	{
		float vertical_acceleration;
		Bytes >> vertical_acceleration;

		int32 effectStartTime;
		Bytes >> effectStartTime;
	}

	if (splineflags & Movement::MoveSplineFlag::Mask_CatmullRom)
	{
		path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(firstSplinePointGame)));

		uint32 pathPointsCnt;
		Bytes >> pathPointsCnt;

		for (uint32 i = 0u; i < pathPointsCnt; i++)
		{
			glm::vec3 gamePathPoint;
			Bytes >> gamePathPoint;
			path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(gamePathPoint)));
		}
		
		//(splineflags.cyclic)
	}
	else // linear
	{
		uint32 count;
		Bytes >> count;

		glm::vec3 lastSplinePointGame;
		Bytes >> lastSplinePointGame;

		path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(firstSplinePointGame)));

		glm::vec3 middle = (firstSplinePointGame + lastSplinePointGame) / 2.0f;

		for (uint32 i = 1; i < count; ++i)
		{
			uint32 packedPos;
			Bytes >> packedPos;

			int32_t packedXX = (int)packedPos >>  0 & 0x7FF;
			int32_t packedYY = (int)packedPos >> 11 & 0x7FF;
			int32_t packedZZ = (int)packedPos >> 22 & 0x3FF;
			
			if (packedXX > 0x400)
				packedXX -= 0x800;
			if (packedYY > 0x400)
				packedYY -= 0x800;
			if (packedZZ > 0x200)
				packedZZ -= 0x400;

			float x2 = float(packedXX) / 4.0f;
			float y2 = float(packedYY) / 4.0f;
			float z2 = float(packedZZ) / 4.0f;

			glm::vec3 pOffs(x2, y2, z2);
			glm::vec3 point = fromGameToReal(middle - pOffs);

			path->AddPathNode(MakeShared(CWoWPathNode, point));
		}

		path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(lastSplinePointGame)));
	}

	m_WoWPath = path;
	CommitPositionAndRotation();
}

void CowServerUnit::OnValuesUpdated(const UpdateMask & Mask)
{
	if (Mask.GetBit(OBJECT_FIELD_SCALE_X))
	{
		m_DisplayID_Scale = m_Values.GetFloatValue(OBJECT_FIELD_SCALE_X);
		m_DisplayID_Scale_IsDirty = true;
	}

	if (Mask.GetBit(UNIT_FIELD_DISPLAYID))
	{
		OnDisplayIDChanged(m_Values.GetUInt32Value(UNIT_FIELD_DISPLAYID));
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID_MAINHAND))
	{
		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<CCharacter>(m_DisplayID_ModelInstance))
			hidderNodeAsCharacter->SetItem(EQUIPMENT_SLOT_MAINHAND, GetItemDisplayInfoIDByItemID(m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID_MAINHAND), 0));
		else
			Log::Warn("UNIT_VIRTUAL_ITEM_SLOT_ID_MAINHAND Error.");
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID_OFFHAND))
	{
		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<CCharacter>(m_DisplayID_ModelInstance))
			hidderNodeAsCharacter->SetItem(EQUIPMENT_SLOT_OFFHAND, GetItemDisplayInfoIDByItemID(m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID_OFFHAND), 0));
		else
			Log::Warn("UNIT_VIRTUAL_ITEM_SLOT_ID_OFFHAND Error.");
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID_RANGED))
	{
		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<CCharacter>(m_DisplayID_ModelInstance))
			hidderNodeAsCharacter->SetItem(EQUIPMENT_SLOT_RANGED, GetItemDisplayInfoIDByItemID(m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID_RANGED), 0));
		else
			Log::Warn("UNIT_VIRTUAL_ITEM_SLOT_ID_RANGED Error.");
	}

	if (Mask.GetBit(UNIT_FIELD_MOUNTDISPLAYID))
	{
		uint32 mountDisplayID = m_Values.GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID);
		if (mountDisplayID != 0)
		{
			Mount_OnMounted(mountDisplayID);
		}
		else
		{
			Mount_OnDismounted();
		}
	}
}

void CowServerUnit::OnHiddenNodePositionChanged()
{
	if (m_DisplayID_ModelInstance)
	{
		m_DisplayID_ModelInstance->SetLocalPosition(Position);
		m_DisplayID_ModelInstance->SetLocalRotationEuler(glm::vec3(0.0f, Orientation, 0.0f));
	}

	if (m_Mount_IsMounted)
	{
		m_Mount_ModelInstance->SetLocalPosition(Position);
		m_Mount_ModelInstance->SetLocalRotationEuler(glm::vec3(0.0f, Orientation, 0.0f));

		if (m_DisplayID_ModelInstance)
		{
			m_DisplayID_ModelInstance->SetLocalPosition(glm::vec3(0.0f));
			m_DisplayID_ModelInstance->SetLocalRotationEuler(glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
}


Race CowServerUnit::GetRace() const 
{ 
	if (false == m_Values.IsExists(UNIT_FIELD_BYTES_0))
		throw CException("Value don't exists.");
	return (Race)m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE); 
}

Class CowServerUnit::GetClass() const 
{ 
	if (false == m_Values.IsExists(UNIT_FIELD_BYTES_0))
		throw CException("Value don't exists.");
	return (Class)m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_CLASS); 
}

Gender CowServerUnit::GetGender() const 
{ 
	if (false == m_Values.IsExists(UNIT_FIELD_BYTES_0))
		throw CException("Value don't exists.");
	return (Gender)m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER); 
}

uint8 CowServerUnit::GetPowerType() const
{
	if (false == m_Values.IsExists(UNIT_FIELD_BYTES_0))
		throw CException("Value don't exists.");
	return m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_POWER_TYPE);
}



void CowServerUnit::OnAnimationEnded(EAnimationID AniamtionID)
{

}

//
// ISceneNode
//
void CowServerUnit::Update(const UpdateEventArgs & e)
{
	__super::Update(e);



	//
	// DisplayID
	//
	if (m_DisplayID_Scale_IsDirty)
	{
		if (m_DisplayID_ModelInstance)
		{
			m_DisplayID_ModelInstance->SetScale(glm::vec3(m_DisplayID_Scale));
			m_DisplayID_Scale_IsDirty = false;
		}
	}



	//
	// Movement
	//
	Movement_HandlePlayerMovement(e);



	//
	// Animation
	//
	Animation_Update();



	//
	// Jump
	//
	if (m_Jump_IsJumpingNow)
	{
		float timeToSeconds = m_MovementInfo.fallTime / 1000.0f;

		// XZ
		{
			glm::vec2 direction(0.0f);
			direction.x = -m_MovementInfo.jump.cosAngle; // y
			direction.y = -m_MovementInfo.jump.sinAngle; // x
			direction = glm::normalize(direction);

			glm::vec2 xz = m_JumpXZ0 + direction * (m_MovementInfo.jump.xyspeed * timeToSeconds);
			Position.xz = xz;
		}

		// Y
		{
			float speedPerTick = (-1.0f) * m_MovementInfo.jump.zspeed;
			float gravityPerTick = (-1.0f) * (gravity);

			float y = m_Jump_y0 + (speedPerTick * timeToSeconds) + ((gravityPerTick * timeToSeconds * timeToSeconds) / 2.0f);
			Position.y = y;
		}

		m_MovementInfo.fallTime += e.DeltaTime;

		CommitPositionAndRotation();
	}


	//
	// Mount
	//
	if (m_Mount_IsDirty)
	{
		if (m_Mount_IsMounted)
		{
			if (m_Mount_ModelInstance && m_Mount_ModelInstance->IsLoaded())
			{
				if (m_DisplayID_ModelInstance && m_DisplayID_ModelInstance->IsLoaded())
				{
					m_DisplayID_ModelInstance->GetAnimatorComponent()->PlayAnimation(EAnimationID::Mount, true);
					m_DisplayID_ModelInstance->Attach(EM2_AttachmentPoint::MountMain);

					m_Mount_ModelInstance->AddChild(m_DisplayID_ModelInstance);
					CommitPositionAndRotation();

					m_Mount_IsDirty = false;
				}
			}
		}
		else
		{
			if (m_DisplayID_ModelInstance && m_DisplayID_ModelInstance->IsLoaded())
			{
				m_DisplayID_ModelInstance->GetAnimatorComponent()->PlayAnimation(EAnimationID::Stand, true);
				m_DisplayID_ModelInstance->Detach();

				if (m_DisplayID_ModelInstance->GetParent() != GetScene().GetRootSceneNode())
					GetScene().GetRootSceneNode()->AddChild(m_DisplayID_ModelInstance);
				CommitPositionAndRotation();

				m_Mount_IsDirty = false;
			}
		}
	}

}



//
// Protected
//
void CowServerUnit::ReadMovementInfoPacket(CServerPacket & Bytes)
{
	Bytes >> m_MovementInfo.flags;
	Bytes >> m_MovementInfo.flags2;

	Bytes >> m_MovementInfo.time;
	Bytes >> m_MovementInfo.pos;
	Bytes >> m_MovementInfo.orientation;

	if (m_MovementInfo.HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
	{
		Bytes.ReadPackedGuid(&m_MovementInfo.transport.guid);

		Bytes >> m_MovementInfo.transport.pos;
		m_MovementInfo.transport.pos = fromGameToReal(m_MovementInfo.transport.pos);

		Bytes >> m_MovementInfo.transport.orientation;
		m_MovementInfo.transport.orientation = glm::degrees(m_MovementInfo.transport.orientation + glm::half_pi<float>());

		Bytes >> m_MovementInfo.transport.time;

		Bytes >> m_MovementInfo.transport.seat;

		if (m_MovementInfo.HasExtraMovementFlag(MOVEMENTFLAG2_INTERPOLATED_MOVEMENT))
		{
			Bytes >> m_MovementInfo.transport.time2;
		}
	}
	else
	{
		TransportID = CowGuid(0ull);
	}

	if ((m_MovementInfo.HasMovementFlag(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (m_MovementInfo.HasExtraMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)))
	{
		Bytes >> m_MovementInfo.pitch;
	}

	Bytes >> m_MovementInfo.fallTime;
	//Log::Info("Flags '%d', '%d', Fall time = '%d'.", m_MovementFlags, m_MovementFlagsExtra, m_FallTime);

	if (m_MovementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING))
	{
		Bytes >> m_MovementInfo.jump.zspeed;
		Bytes >> m_MovementInfo.jump.sinAngle;
		Bytes >> m_MovementInfo.jump.cosAngle;
		Bytes >> m_MovementInfo.jump.xyspeed;

		if (false == m_Jump_IsJumpingNow)
		{
			m_JumpXZ0 = glm::vec2(Position.x, Position.z);
			m_Jump_y0 = Position.y;		
		}

		m_Jump_IsJumpingNow = true;
	}
	else
	{
		m_Jump_IsJumpingNow = false;
	}

	if (m_MovementInfo.HasMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION))
	{
		Bytes >> m_MovementInfo.splineElevation;
	}

	Position = fromGameToReal(m_MovementInfo.pos);
	Orientation = glm::degrees(m_MovementInfo.orientation + glm::half_pi<float>());
	CommitPositionAndRotation();
}



std::shared_ptr<CowServerUnit> CowServerUnit::Create(CowServerWorld& WoWWorld, IScene& Scene, CowGuid Guid)
{
	std::shared_ptr<CowServerUnit> thisObj = MakeShared(CowServerUnit, Scene, WoWWorld, Guid);
	//Log::Error("CowServerUnit created. ID  = %d. HIGH = %d, ENTRY = %d, COUNTER = %d", Guid.GetRawValue(), Guid.GetHigh(), Guid.GetEntry(), Guid.GetCounter());
	return thisObj;
}

void CowServerUnit::Destroy()
{
	if (auto model = m_DisplayID_ModelInstance)
	{
		model->MakeMeOrphan();
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(model);
	}

	if (auto model = m_Mount_ModelInstance)
	{
		model->MakeMeOrphan();
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(model);
	}
}

SCharacterItemTemplate CowServerUnit::GetItemDisplayInfoIDByItemID(uint32 ItemID, uint32 EnchantID)
{
	auto itemRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_Item()[ItemID];
	if (itemRecord == nullptr)
	{
		Log::Warn("CowServerUnit::GetItemDisplayInfoIDByItemID: Item don't contains id '%d'.", ItemID);
		return SCharacterItemTemplate();
	}

	return SCharacterItemTemplate(itemRecord->Get_DisplayInfoID(), itemRecord->Get_InventorySlot(), EnchantID);
}



//
// Protected
//



//
// DisplayID functional
//
void CowServerUnit::OnDisplayIDChanged(uint32 DisplayID)
{
	if (m_DisplayID_ModelInstance != nullptr)
		return;

	CWorldObjectCreator creator(GetScene().GetBaseManager());
	DisplayID_SetModelInstance(creator.BuildCreatureFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), DisplayID));

	//const DBC_CreatureDisplayInfoRecord * creatureDisplayInfo = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo()[diplayID];
	//if (creatureDisplayInfo == nullptr)
	//	throw CException("Creature display info '%d' don't found.", displayInfo);
	//const DBC_CreatureModelDataRecord* creatureModelDataRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureModelData()[creatureDisplayInfo->Get_Model()];
	//if (creatureModelDataRecord == nullptr)
	//	throw CException("Creature model data '%d' don't found.", creatureDisplayInfo->Get_Model());
	//float scaleFromCreature = creatureDisplayInfo->Get_Scale();
	//float scaleFromModel = creatureModelDataRecord->Get_Scale();
}

std::shared_ptr<CCreature> CowServerUnit::DisplayID_GetModelInstance() const
{
	return m_DisplayID_ModelInstance;
}

void CowServerUnit::DisplayID_SetModelInstance(std::shared_ptr<CCreature> ModelInstance)
{
	m_DisplayID_ModelInstance = ModelInstance;
	m_DisplayID_ModelInstance->GetComponentT<CM2AnimatorComponent>()->SetAnimationEventListener(std::dynamic_pointer_cast<IM2AnimationEventsListener>(shared_from_this()));
}



//
// Movement functional
//
void CowServerUnit::Movement_HandlePlayerMovement(const UpdateEventArgs& e)
{
	if (m_WoWPath == nullptr)
		return;

	m_WoWPath->AddCurrTime(e.DeltaTime);

	glm::vec3 NextPoint = m_WoWPath->GetPositionByCurrTime();
	if (glm::distance(NextPoint, Position) > 0.01f)
	{
		glm::vec3 test = m_WoWPath->GetNextNodePosition();

		glm::vec3 directionVec = glm::normalize(glm::vec3(test.x, 0.0f, test.z) - glm::vec3(Position.x, 0.0f, Position.z));
		float yaw = atan2(directionVec.x, directionVec.z);

		Position = NextPoint;
		Orientation = glm::degrees(yaw - glm::half_pi<float>());
		CommitPositionAndRotation();

		m_Movement_IsMoveNow = true;
	}
	else
	{
		m_Movement_IsMoveNow = false;
	}
}

const SMovementInfo& CowServerUnit::Movement_GetMovementInfo() const
{
	return m_MovementInfo;
}



//
// Animation functional
//
void CowServerUnit::Animation_Play(EAnimationID Animation, bool IsLoop)
{
	if (m_Mount_ModelInstance && m_Mount_ModelInstance->IsLoaded())
	{
		m_Mount_ModelInstance->GetAnimatorComponent()->PlayAnimation(Animation, IsLoop);
	}
	else if (m_DisplayID_ModelInstance && m_DisplayID_ModelInstance->IsLoaded())
	{
		m_DisplayID_ModelInstance->GetAnimatorComponent()->PlayAnimation(Animation, IsLoop);
	}
}

EAnimationID CowServerUnit::Animation_GetCurrentAnimation() const
{
	if (m_Mount_IsMounted)
	{
		if (m_Mount_ModelInstance && m_Mount_ModelInstance->IsLoaded())
		{
			return m_Mount_ModelInstance->GetAnimatorComponent()->GetCurrentAnimationID();
		}
		else if (m_DisplayID_ModelInstance && m_DisplayID_ModelInstance->IsLoaded())
		{
			return 	m_DisplayID_ModelInstance->GetAnimatorComponent()->GetCurrentAnimationID();
		}
	}
	else
	{
		if (m_DisplayID_ModelInstance && m_DisplayID_ModelInstance->IsLoaded())
		{
			return 	m_DisplayID_ModelInstance->GetAnimatorComponent()->GetCurrentAnimationID();
		}
	}

	return EAnimationID::Stand;
}

void CowServerUnit::Animation_Update()
{
	if (m_Movement_IsMoveNow)
	{
		Animation_Play(EAnimationID::Walk, true);
		return;
	}

	if (m_Jump_IsJumpingNow)
	{
		if (Animation_GetCurrentAnimation() != EAnimationID::JumpStart)
			Animation_Play(EAnimationID::JumpStart, false);
		return;
	}

	Animation_Play(EAnimationID::Stand, true);
}



//
// Mount functional
//
void CowServerUnit::Mount_OnMounted(uint32 MountDisplayID)
{
	if (m_Mount_IsMounted)
		return;

	// Delete old mount model
	if (m_Mount_ModelInstance != nullptr)
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_Mount_ModelInstance);

	CWorldObjectCreator creator(GetScene().GetBaseManager());
	m_Mount_ModelInstance = creator.BuildCreatureFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), MountDisplayID);
	m_Mount_ModelInstance->GetComponentT<CM2AnimatorComponent>()->SetAnimationEventListener(std::dynamic_pointer_cast<IM2AnimationEventsListener>(shared_from_this()));

	m_Mount_IsMounted = true;
	m_Mount_IsDirty = true;
}

void CowServerUnit::Mount_OnDismounted()
{
	if (false == m_Mount_IsMounted)
		return;
	
	if (m_Mount_ModelInstance != nullptr)
	{
		m_Mount_ModelInstance->MakeMeOrphan();
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_Mount_ModelInstance);
		m_Mount_ModelInstance = nullptr;
	}

	m_Mount_IsMounted = false;
	m_Mount_IsDirty = true;
}

#endif
