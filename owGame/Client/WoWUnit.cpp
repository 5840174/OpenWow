#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWUnit.h"

// Additional
#include "World/WorldObjectsCreator.h"

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


WoWUnit::WoWUnit(IScene& Scene, CWoWObjectGuid Guid)
	: CWoWWorldObject(Scene, Guid)
	, DestinationPoint(0.0f)
{
	m_ObjectType |= TYPEMASK_UNIT;
	m_ObjectTypeId = TYPEID_UNIT;
	m_valuesCount = UNIT_END;
}

WoWUnit::~WoWUnit()
{
}

void WoWUnit::ProcessMovementPacket(CByteBuffer & Bytes)
{
	Bytes >> m_MovementFlags;
	Bytes >> m_MovementFlagsExtra;

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
		Bytes.ReadPackedUInt64(m_Transport.guid);
		m_Transport.pos.x = Bytes.ReadFloat();
		m_Transport.pos.y = Bytes.ReadFloat();
		m_Transport.pos.z = Bytes.ReadFloat();
		m_Transport.pos.w = Bytes.ReadFloat();
		Bytes >> uint32(m_Transport.time);
		Bytes >> int8(m_Transport.seat);

		if (HasExtraMovementFlag(MOVEMENTFLAG2_INTERPOLATED_MOVEMENT))
			Bytes >> uint32(m_Transport.time2);
	}

	// 0x02200000
	if ((HasMovementFlag(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (HasExtraMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)))
	{
		Bytes >> float(m_Pitch);
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
		Bytes >> float(m_SplineElevation);

	CommitPositionAndRotation();
}

void WoWUnit::ProcessMonsterMove(CByteBuffer & Bytes)
{
	glm::vec3 gamePosition;
	Bytes >> gamePosition.x;
	Bytes >> gamePosition.y;
	Bytes >> gamePosition.z;
	Position = fromGameToReal(gamePosition);

	uint32 splineID;
	Bytes >> splineID;

	MonsterMoveType monsterMoveType;
	Bytes.read(&monsterMoveType);

	switch (monsterMoveType)
	{
		case MonsterMoveType::MonsterMoveFacingSpot:
		{
			float x, y, z;
			Bytes >> x;
			Bytes >> y;
			Bytes >> z;
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
			return;
	}

	Movement::MoveSplineFlag splineflags;
	Bytes.read(&splineflags);

	if (splineflags.animation)
	{
		uint8 animationID;
		Bytes >> animationID;

		int32 effectStartTime;
		Bytes >> effectStartTime;
	}

	int32 duration;
	Bytes >> duration;

	if (splineflags.parabolic)
	{
		float vertical_acceleration;
		Bytes >> vertical_acceleration;

		int32 effectStartTime;
		Bytes >> effectStartTime;
	}


	if (splineflags & Movement::MoveSplineFlag::Mask_CatmullRom)
	{
		uint32 pathPointsCnt;
		Bytes >> pathPointsCnt;

		for (uint32 i = 0u; i < pathPointsCnt; i++)
		{
			glm::vec3 gamePathPoint;
			Bytes >> gamePathPoint.x;
			Bytes >> gamePathPoint.y;
			Bytes >> gamePathPoint.z;
		}

		//(splineflags.cyclic)
	}
	else // linear
	{
		uint32 count;
		Bytes >> count;

		glm::vec3 gameLastPoint;
		Bytes >> gameLastPoint.x;
		Bytes >> gameLastPoint.y;
		Bytes >> gameLastPoint.z;
		DestinationPoint = fromGameToReal(gameLastPoint);

		if (count > 1)
		{
			for (uint32 i = 1; i < count; ++i)
			{
				uint32 packedPos;
				Bytes >> packedPos;

				//G3D::Vector3 middle = (real_path[0] + real_path[last_idx]) / 2.f;
				//G3D::Vector3 offset;
				// first and last points already appended
				//for (uint32 i = 1; i < last_idx; ++i)
				//{
				//	offset = middle - real_path[i];

				// this is offsets to path
				float x2 = ((packedPos) & 0x7FF) * 0.25;
				float y2 = ((packedPos >> 11) & 0x7FF) * 0.25;
				float z2 = ((packedPos >> 22) & 0x3FF) * 0.25;
			}
		}
	}

	CommitPositionAndRotation();
}



float WoWUnit::GetSpeed(UnitMoveType MoveType) const
{
	return m_Speed[MoveType];
}

void WoWUnit::SetSpeed(UnitMoveType MoveType, float Speed)
{
	m_Speed[MoveType] = Speed;
}




//
// ISceneNode
//
void WoWUnit::Update(const UpdateEventArgs & e)
{
	__super::Update(e);

	if (DestinationPoint.x == 0.0f || DestinationPoint.y == 0.0f || DestinationPoint.z == 0.0f)
		return;

	if (m_HiddenNode == nullptr)
		return;

	if (glm::distance(Position, DestinationPoint) > 0.1f)
	{
		glm::vec3 directionVec = glm::normalize(DestinationPoint - Position);

		float speed = GetSpeed(MOVE_WALK) / 1000.0f * e.DeltaTime;
		Position = Position + directionVec * speed;
		CommitPositionAndRotation();

		glm::vec3 rightDirection = glm::normalize(glm::cross(directionVec, glm::vec3(0.0f, 1.0f, 0.0f)));
		glm::vec3 leftDirection = -rightDirection;

		glm::quat lookAtQuat = glm::quatLookAt(leftDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		//SetLocalRotationQuaternion(lookAtQuat);
	}
}



//
// Protected
//
std::shared_ptr<WoWUnit> WoWUnit::Create(IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWUnit> thisObj = MakeShared(WoWUnit, Scene, Guid);
	//Log::Error("WoWUnit created. ID  = %d. HIGH = %d, ENTRY = %d, COUNTER = %d", Guid.GetRawValue(), Guid.GetHigh(), Guid.GetEntry(), Guid.GetCounter());
	return thisObj;
}

void WoWUnit::AfterCreate(IScene& Scene)
{
	if (m_HiddenNode != nullptr)
	{
		Log::Error("WoWUnit: Try to call 'AfterCreate' for object.");
		return;
	}

	uint32 displayInfo = GetUInt32Value(UNIT_FIELD_DISPLAYID);
	if (displayInfo != 0)
	{
		CWorldObjectCreator creator(Scene.GetBaseManager());
		m_HiddenNode = creator.BuildCreatureFromDisplayInfo(Scene.GetBaseManager().GetApplication().GetRenderDevice(), &Scene, displayInfo);

		const DBC_CreatureDisplayInfoRecord * creatureDisplayInfo = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo()[displayInfo];
		if (creatureDisplayInfo == nullptr)
			throw CException("Creature display info '%d' don't found.", displayInfo);

		const DBC_CreatureModelDataRecord* creatureModelDataRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureModelData()[creatureDisplayInfo->Get_Model()];
		if (creatureModelDataRecord == nullptr)
			throw CException("Creature model data '%d' don't found.", creatureDisplayInfo->Get_Model());

		float scaleFromCreature = creatureDisplayInfo->Get_Scale();
		float scaleFromModel = creatureModelDataRecord->Get_Scale();
		float scale = GetFloatValue(OBJECT_FIELD_SCALE_X);



		//m_HiddenNode->SetScale(glm::vec3(scale));
	}
	else
		throw CException("GameObject display info is zero.");
}

void WoWUnit::Destroy()
{
	if (m_HiddenNode)
		m_HiddenNode->MakeMeOrphan();
}

#endif
