#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWUnit.h"

// Additional
#include "../World.h"

namespace
{
	enum MonsterMoveType : uint8
	{
		MonsterMoveNormal = 0,
		MonsterMoveStop = 1,
		MonsterMoveFacingSpot = 2,
		MonsterMoveFacingTarget = 3,
		MonsterMoveFacingAngle = 4
	};
}


WoWUnit::WoWUnit(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid)
	: CWoWWorldObject(Scene, WoWWorld, Guid)
{
	m_ObjectType |= TYPEMASK_UNIT;
	m_Values.SetValuesCount(UNIT_END);
}

WoWUnit::~WoWUnit()
{
}

void WoWUnit::ProcessMovementPacket(CByteBuffer & Bytes)
{
	Bytes.read(&m_MovementFlags);
	Bytes.read(&m_MovementFlagsExtra);

	uint32 timeMS;
	Bytes >> timeMS;

	glm::vec3 gamePosition;
	Bytes >> gamePosition.x;
	Bytes >> gamePosition.y;
	Bytes >> gamePosition.z;
	Position = fromGameToReal(gamePosition);

	float gameOrientation;
	Bytes >> gameOrientation;
	Orientation = glm::degrees(gameOrientation + glm::half_pi<float>());

	// 0x00000200
	if (HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
	{
		uint64 transportGuid;
		Bytes.ReadPackedUInt64(transportGuid);
		TransportID = CWoWGuid(transportGuid);

		glm::vec3 gamePositionTransportOffset;
		Bytes >> gamePositionTransportOffset.x;
		Bytes >> gamePositionTransportOffset.y;
		Bytes >> gamePositionTransportOffset.z;
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

	// 0x02200000
	if ((HasMovementFlag(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (HasExtraMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)))
	{
		Bytes >> float(m_StrideOrPitch);
	}

	Bytes >> uint32(m_FallTime);

	// 0x00001000
	if (HasMovementFlag(MOVEMENTFLAG_FALLING))
	{
		Bytes >> float(m_Jump.zspeed);
		Bytes >> float(m_Jump.sinAngle);
		Bytes >> float(m_Jump.cosAngle);
		Bytes >> float(m_Jump.xyspeed);
	}

	// 0x04000000
	if (HasMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION))
	{
		Bytes >> float(m_SplineElevation);
	}

	SetSpeed(MOVE_WALK, Bytes.ReadFloat());
	SetSpeed(MOVE_RUN, Bytes.ReadFloat());
	SetSpeed(MOVE_RUN_BACK, Bytes.ReadFloat());
	SetSpeed(MOVE_SWIM, Bytes.ReadFloat());
	SetSpeed(MOVE_SWIM_BACK, Bytes.ReadFloat());
	SetSpeed(MOVE_FLIGHT, Bytes.ReadFloat());
	SetSpeed(MOVE_FLIGHT_BACK, Bytes.ReadFloat());
	SetSpeed(MOVE_TURN_RATE, Bytes.ReadFloat());
	SetSpeed(MOVE_PITCH_RATE, Bytes.ReadFloat());

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

void WoWUnit::ProcessMonsterMove(CByteBuffer & Bytes)
{
	glm::vec3 firstSplinePointGame;
	Bytes >> firstSplinePointGame;
	Position = fromGameToReal(firstSplinePointGame);

	uint32 splineID;
	Bytes >> splineID;

	MonsterMoveType monsterMoveType;
	Bytes >> monsterMoveType;

	switch (monsterMoveType)
	{
		case MonsterMoveType::MonsterMoveFacingSpot:
		{
			glm::vec3 targetPoint;
			Bytes >> targetPoint;
		}
		break;

		case MonsterMoveType::MonsterMoveFacingTarget:
		{
			uint64 targetGUID;
			Bytes >> targetGUID;
		}
		break;

		case MonsterMoveType::MonsterMoveFacingAngle:
		{
			float angle;
			Bytes >> angle;
		}
		break;

		case MonsterMoveType::MonsterMoveNormal:
		{
			// normal packet
		}
		break;

		case MonsterMoveType::MonsterMoveStop:
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
	{
		uint32 diplayID = m_Values.GetUInt32Value(UNIT_FIELD_DISPLAYID);

		if (m_HiddenNode != nullptr)
		{
			//Log::Warn("WoWUnit: UNIT_FIELD_DISPLAYID updated, but Node already exists.");
			return;
		}

		CWorldObjectCreator creator(GetScene().GetBaseManager());
		m_HiddenNode = creator.BuildCreatureFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), diplayID);

		//const DBC_CreatureDisplayInfoRecord * creatureDisplayInfo = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo()[diplayID];
		//if (creatureDisplayInfo == nullptr)
		//	throw CException("Creature display info '%d' don't found.", displayInfo);

		//const DBC_CreatureModelDataRecord* creatureModelDataRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureModelData()[creatureDisplayInfo->Get_Model()];
		//if (creatureModelDataRecord == nullptr)
		//	throw CException("Creature model data '%d' don't found.", creatureDisplayInfo->Get_Model());

		//float scaleFromCreature = creatureDisplayInfo->Get_Scale();
		//float scaleFromModel = creatureModelDataRecord->Get_Scale();
		float scale = m_Values.GetFloatValue(OBJECT_FIELD_SCALE_X);
		m_HiddenNode->SetScale(glm::vec3(scale));
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID + 0))
	{
		uint32 mainHandDisplayID = m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0);
		if (mainHandDisplayID != 0)
			if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
				hidderNodeAsCharacter->Template().ItemsTemplates[EQUIPMENT_SLOT_MAINHAND] = GetItemDisplayInfoIDByItemID(mainHandDisplayID);

		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
			hidderNodeAsCharacter->RefreshItemVisualData();
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID + 1))
	{
		uint32 offHandDisplayID = m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1);
		if (offHandDisplayID != 0)
			if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
				hidderNodeAsCharacter->Template().ItemsTemplates[EQUIPMENT_SLOT_OFFHAND] = GetItemDisplayInfoIDByItemID(offHandDisplayID);

		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
			hidderNodeAsCharacter->RefreshItemVisualData();
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID + 2))
	{
		uint32 rangedDisplayID = m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2);
		if (rangedDisplayID != 0)
			if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
				hidderNodeAsCharacter->Template().ItemsTemplates[EQUIPMENT_SLOT_RANGED] = GetItemDisplayInfoIDByItemID(rangedDisplayID);

		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
			hidderNodeAsCharacter->RefreshItemVisualData();
	}
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
}



//
// Protected
//
std::shared_ptr<WoWUnit> WoWUnit::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid)
{
	std::shared_ptr<WoWUnit> thisObj = MakeShared(WoWUnit, Scene, WoWWorld, Guid);
	//Log::Error("WoWUnit created. ID  = %d. HIGH = %d, ENTRY = %d, COUNTER = %d", Guid.GetRawValue(), Guid.GetHigh(), Guid.GetEntry(), Guid.GetCounter());
	return thisObj;
}

void WoWUnit::Destroy()
{
	if (m_HiddenNode)
		m_HiddenNode->MakeMeOrphan();
}

CInet_ItemTemplate WoWUnit::GetItemDisplayInfoIDByItemID(uint32 ItemID)
{
	auto itemRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_Item()[ItemID];
	if (itemRecord == nullptr)
	{
		Log::Warn("WoWUnit::GetItemDisplayInfoIDByItemID: Item don't contains id '%d'.", ItemID);
		return CInet_ItemTemplate();
	}

	return CInet_ItemTemplate(itemRecord->Get_DisplayInfoID(), itemRecord->Get_InventorySlot(), 0);
}

#endif
