#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWUnit.h"

// Additional
#include "../../World/World.h"

float gravity = static_cast<float>(19.29110527038574);

WoWUnit::WoWUnit(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid)
	: CWoWWorldObject(Scene, WoWWorld, Guid)
	, m_Mount_IsMounted(false)
	, m_Mount_IsDirty(false)
{
	//m_ObjectType |= TYPEMASK_UNIT;
	m_Values.SetValuesCount(UNIT_END);
}

WoWUnit::~WoWUnit()
{
	if (auto model = m_UnitModel)
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(model);

	if (auto model = m_Mount_Creature)
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(model);
}

void WoWUnit::ProcessMovementPacket(CServerPacket & Bytes)
{
	ReadMovementInfoPacket(Bytes);

	SetSpeed(UnitSpeedType::MOVE_WALK, Bytes.ReadFloat());
	SetSpeed(UnitSpeedType::MOVE_RUN, Bytes.ReadFloat());
	SetSpeed(UnitSpeedType::MOVE_RUN_BACK, Bytes.ReadFloat());
	SetSpeed(UnitSpeedType::MOVE_SWIM, Bytes.ReadFloat());
	SetSpeed(UnitSpeedType::MOVE_SWIM_BACK, Bytes.ReadFloat());
	SetSpeed(UnitSpeedType::MOVE_FLIGHT, Bytes.ReadFloat());
	SetSpeed(UnitSpeedType::MOVE_FLIGHT_BACK, Bytes.ReadFloat());
	SetSpeed(UnitSpeedType::MOVE_TURN_RATE, Bytes.ReadFloat());
	SetSpeed(UnitSpeedType::MOVE_PITCH_RATE, Bytes.ReadFloat());

	if (HasMovementFlag(MOVEMENTFLAG_SPLINE_ENABLED))
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

void WoWUnit::Do_MonsterMove(CServerPacket& Bytes)
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

void WoWUnit::OnValuesUpdated(const UpdateMask & Mask)
{
	if (Mask.GetBit(UNIT_FIELD_DISPLAYID))
		OnDisplayIDChanged(m_Values.GetUInt32Value(UNIT_FIELD_DISPLAYID));

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID_MAINHAND))
	{
		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<CCharacter>(m_UnitModel))
			hidderNodeAsCharacter->SetItem(EQUIPMENT_SLOT_MAINHAND, GetItemDisplayInfoIDByItemID(m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID_MAINHAND), 0));
		else
			Log::Warn("UNIT_VIRTUAL_ITEM_SLOT_ID_MAINHAND Error.");
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID_OFFHAND))
	{
		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<CCharacter>(m_UnitModel))
			hidderNodeAsCharacter->SetItem(EQUIPMENT_SLOT_OFFHAND, GetItemDisplayInfoIDByItemID(m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID_OFFHAND), 0));
		else
			Log::Warn("UNIT_VIRTUAL_ITEM_SLOT_ID_OFFHAND Error.");
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID_RANGED))
	{
		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<CCharacter>(m_UnitModel))
			hidderNodeAsCharacter->SetItem(EQUIPMENT_SLOT_RANGED, GetItemDisplayInfoIDByItemID(m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID_RANGED), 0));
		else
			Log::Warn("UNIT_VIRTUAL_ITEM_SLOT_ID_RANGED Error.");
	}

	if (Mask.GetBit(UNIT_FIELD_MOUNTDISPLAYID))
	{
		uint32 mountDisplayID = m_Values.GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID);
		if (mountDisplayID != 0)
		{
			OnMounted(mountDisplayID);
		}
		else
		{
			OnDismounted();
		}
	}
}

void WoWUnit::OnHiddenNodePositionChanged()
{
	if (m_UnitModel)
	{
		m_UnitModel->SetLocalPosition(Position);
		m_UnitModel->SetLocalRotationEuler(glm::vec3(0.0f, Orientation, 0.0f));
	}

	if (m_Mount_IsMounted)
	{
		m_Mount_Creature->SetLocalPosition(Position);
		m_Mount_Creature->SetLocalRotationEuler(glm::vec3(0.0f, Orientation, 0.0f));

		if (m_UnitModel)
		{
			m_UnitModel->SetLocalPosition(glm::vec3(0.0f));
			m_UnitModel->SetLocalRotationEuler(glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
}


Race WoWUnit::GetRace() const 
{ 
	if (false == m_Values.IsExists(UNIT_FIELD_BYTES_0))
		throw CException("Value don't exists.");
	return (Race)m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_RACE); 
}

Class WoWUnit::GetClass() const 
{ 
	if (false == m_Values.IsExists(UNIT_FIELD_BYTES_0))
		throw CException("Value don't exists.");
	return (Class)m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_CLASS); 
}

Gender WoWUnit::GetGender() const 
{ 
	if (false == m_Values.IsExists(UNIT_FIELD_BYTES_0))
		throw CException("Value don't exists.");
	return (Gender)m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_GENDER); 
}

uint8 WoWUnit::GetPowerType() const
{
	if (false == m_Values.IsExists(UNIT_FIELD_BYTES_0))
		throw CException("Value don't exists.");
	return m_Values.GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_POWER_TYPE);
}



//
// ISceneNode
//
void WoWUnit::Update(const UpdateEventArgs & e)
{
	__super::Update(e);

	if (m_WoWPath)
	{
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
		}
	}



	//
	// Mount
	//
	if (m_Mount_IsDirty)
	{
		if (m_Mount_IsMounted)
		{
			if (m_Mount_Creature && m_Mount_Creature->IsLoaded())
			{
				if (m_UnitModel && m_UnitModel->IsLoaded())
				{
					m_UnitModel->GetAnimatorComponent()->PlayAnimation(91, true);
					m_UnitModel->Attach(EM2_AttachmentPoint::MountMain);

					m_Mount_Creature->AddChild(m_UnitModel);
					CommitPositionAndRotation();

					m_Mount_IsDirty = false;
				}
			}
		}
		else
		{
			if (m_UnitModel && m_UnitModel->IsLoaded())
			{
				m_UnitModel->GetAnimatorComponent()->PlayAnimation(0, true);
				m_UnitModel->Detach();

				if (m_UnitModel->GetParent() != GetScene().GetRootSceneNode())
					GetScene().GetRootSceneNode()->AddChild(m_UnitModel);
				CommitPositionAndRotation();

				m_Mount_IsDirty = false;
			}
		}
	}
}



//
// Protected
//
void WoWUnit::ReadMovementInfoPacket(CServerPacket & Bytes)
{
	Bytes.read(&m_MovementFlags);
	Bytes.read(&m_MovementFlagsExtra);

	uint32 gameTimeMS;
	Bytes >> gameTimeMS;

	glm::vec3 gamePosition;
	Bytes >> gamePosition;
	Position = fromGameToReal(gamePosition);

	float gameOrientation;
	Bytes >> gameOrientation;
	Orientation = glm::degrees(gameOrientation + glm::half_pi<float>());

	if (HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
	{
		CWoWGuid transportGuid;
		Bytes.ReadPackedGuid(&transportGuid);
		TransportID = transportGuid;

		glm::vec3 gamePositionTransportOffset;
		Bytes >> gamePositionTransportOffset;
		PositionTransportOffset = fromGameToReal(gamePositionTransportOffset);

		float gameOrientationTransportOffset;
		Bytes >> gameOrientationTransportOffset;
		OrientationTransportOffset = glm::degrees(gameOrientationTransportOffset + glm::half_pi<float>());

		uint32 transportTime;
		Bytes >> uint32(transportTime);

		int8 transportSeat;
		Bytes >> int8(transportSeat);

		if (HasExtraMovementFlag(MOVEMENTFLAG2_INTERPOLATED_MOVEMENT))
		{
			uint32 transportInterpolatedTime;
			Bytes >> uint32(transportInterpolatedTime);
		}
	}
	else
	{
		TransportID = CWoWGuid(0ull);
	}

	if ((HasMovementFlag(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (HasExtraMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)))
	{
		Bytes >> float(m_StrideOrPitch);
	}

	Bytes >> m_FallTime;

	if (HasMovementFlag(MOVEMENTFLAG_FALLING))
	{
		Bytes >> float(m_Jump.zspeed);
		Bytes >> float(m_Jump.sinAngle);
		Bytes >> float(m_Jump.cosAngle);
		Bytes >> float(m_Jump.xyspeed);
	}

	if (HasMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION))
	{
		Bytes >> float(m_SplineElevation);
	}

	CommitPositionAndRotation();
}



std::shared_ptr<WoWUnit> WoWUnit::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid)
{
	std::shared_ptr<WoWUnit> thisObj = MakeShared(WoWUnit, Scene, WoWWorld, Guid);
	//Log::Error("WoWUnit created. ID  = %d. HIGH = %d, ENTRY = %d, COUNTER = %d", Guid.GetRawValue(), Guid.GetHigh(), Guid.GetEntry(), Guid.GetCounter());
	return thisObj;
}

void WoWUnit::Destroy()
{
	if (auto model = m_UnitModel)
	{
		model->MakeMeOrphan();
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(model);
	}

	if (auto model = m_Mount_Creature)
	{
		model->MakeMeOrphan();
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(model);
	}
}

SCharacterItemTemplate WoWUnit::GetItemDisplayInfoIDByItemID(uint32 ItemID, uint32 EnchantID)
{
	auto itemRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_Item()[ItemID];
	if (itemRecord == nullptr)
	{
		Log::Warn("WoWUnit::GetItemDisplayInfoIDByItemID: Item don't contains id '%d'.", ItemID);
		return SCharacterItemTemplate();
	}

	return SCharacterItemTemplate(itemRecord->Get_DisplayInfoID(), itemRecord->Get_InventorySlot(), EnchantID);
}



//
// Protected
//
void WoWUnit::OnDisplayIDChanged(uint32 DisplayID)
{
	if (m_UnitModel != nullptr)
		return;

	CWorldObjectCreator creator(GetScene().GetBaseManager());
	m_UnitModel = creator.BuildCreatureFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), DisplayID);

	//const DBC_CreatureDisplayInfoRecord * creatureDisplayInfo = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo()[diplayID];
	//if (creatureDisplayInfo == nullptr)
	//	throw CException("Creature display info '%d' don't found.", displayInfo);

	//const DBC_CreatureModelDataRecord* creatureModelDataRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureModelData()[creatureDisplayInfo->Get_Model()];
	//if (creatureModelDataRecord == nullptr)
	//	throw CException("Creature model data '%d' don't found.", creatureDisplayInfo->Get_Model());

	//float scaleFromCreature = creatureDisplayInfo->Get_Scale();
	//float scaleFromModel = creatureModelDataRecord->Get_Scale();
	float scale = 1.0f;
	if (m_Values.IsExists(OBJECT_FIELD_SCALE_X))
		scale = m_Values.GetFloatValue(OBJECT_FIELD_SCALE_X);
	m_UnitModel->SetScale(glm::vec3(scale));
}

void WoWUnit::OnMounted(uint32 MountDisplayID)
{
	if (m_Mount_IsMounted)
		return;

	// Delete old mount model
	if (m_Mount_Creature != nullptr)
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_Mount_Creature);

	CWorldObjectCreator creator(GetScene().GetBaseManager());
	m_Mount_Creature = creator.BuildCreatureFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), MountDisplayID);

	m_Mount_IsMounted = true;
	m_Mount_IsDirty = true;
}

void WoWUnit::OnDismounted()
{
	if (false == m_Mount_IsMounted)
		return;
	
	if (m_Mount_Creature != nullptr)
	{
		m_Mount_Creature->MakeMeOrphan();
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_Mount_Creature);
		m_Mount_Creature = nullptr;
	}

	m_Mount_IsMounted = false;
	m_Mount_IsDirty = true;
}

#endif
