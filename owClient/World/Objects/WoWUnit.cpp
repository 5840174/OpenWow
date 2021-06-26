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


WoWUnit::WoWUnit(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid)
	: CWoWWorldObject(Scene, WoWWorld, Guid)
	, DestinationPoint(0.0f)
{
	m_ObjectType |= TYPEMASK_UNIT;
	m_Values.SetValuesCount(UNIT_END);
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
		uint64 transportGuid;
		Bytes.ReadPackedUInt64(transportGuid);
		TransportID = CWoWObjectGuid(transportGuid);

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
		TransportID = CWoWObjectGuid(0ull);
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
				float x2 = ((packedPos) & 0x7FF) * 0.25f;
				float y2 = ((packedPos >> 11) & 0x7FF) * 0.25f;
				float z2 = ((packedPos >> 22) & 0x3FF) * 0.25f;
			}
		}
	}

	CommitPositionAndRotation();
}

void WoWUnit::OnValueUpdated(uint16 index)
{
	

}

void WoWUnit::OnValuesUpdated(const UpdateMask & Mask)
{
	if (Mask.GetBit(UNIT_FIELD_DISPLAYID))
	{
		if (m_HiddenNode != nullptr)
		{
			Log::Warn("WoWUnit: UNIT_FIELD_DISPLAYID updated, but Node already exists.");
			return;
		}

		CWorldObjectCreator creator(GetScene().GetBaseManager());
		m_HiddenNode = creator.BuildCreatureFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), m_Values.GetUInt32Value(UNIT_FIELD_DISPLAYID));

		/*
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
		*/
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID + 0))
	{
		uint32 mainHandDisplayID = m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0);
		if (mainHandDisplayID != 0)
			if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
				hidderNodeAsCharacter->GetTemplate().ItemsTemplates[EQUIPMENT_SLOT_MAINHAND] = GetItemDisplayInfoIDByItemID(mainHandDisplayID);

		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
			hidderNodeAsCharacter->RefreshItemVisualData();
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID + 1))
	{
		uint32 offHandDisplayID = m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1);
		if (offHandDisplayID != 0)
			if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
				hidderNodeAsCharacter->GetTemplate().ItemsTemplates[EQUIPMENT_SLOT_OFFHAND] = GetItemDisplayInfoIDByItemID(offHandDisplayID);

		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
			hidderNodeAsCharacter->RefreshItemVisualData();
	}

	if (Mask.GetBit(UNIT_VIRTUAL_ITEM_SLOT_ID + 2))
	{
		uint32 rangedDisplayID = m_Values.GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2);
		if (rangedDisplayID != 0)
			if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
				hidderNodeAsCharacter->GetTemplate().ItemsTemplates[EQUIPMENT_SLOT_RANGED] = GetItemDisplayInfoIDByItemID(rangedDisplayID);

		if (auto hidderNodeAsCharacter = std::dynamic_pointer_cast<Character>(m_HiddenNode))
			hidderNodeAsCharacter->RefreshItemVisualData();
	}
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
std::shared_ptr<WoWUnit> WoWUnit::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWUnit> thisObj = MakeShared(WoWUnit, Scene, WoWWorld, Guid);
	//Log::Error("WoWUnit created. ID  = %d. HIGH = %d, ENTRY = %d, COUNTER = %d", Guid.GetRawValue(), Guid.GetHigh(), Guid.GetEntry(), Guid.GetCounter());
	return thisObj;
}

void WoWUnit::AfterCreate(IScene& Scene)
{

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
