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
		Bytes >> float(m_SplineElevation);

	CommitPositionAndRotation();
}

void WoWUnit::ProcessMonsterMove(CByteBuffer & Bytes)
{
	glm::vec3 firstSplinePointGame;
	Bytes >> firstSplinePointGame.x;
	Bytes >> firstSplinePointGame.y;
	Bytes >> firstSplinePointGame.z;
	Position = fromGameToReal(firstSplinePointGame);

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
		auto path = MakeShared(CWoWPath);

		path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(firstSplinePointGame)));

		uint32 pathPointsCnt;
		Bytes >> pathPointsCnt;

		for (uint32 i = 0u; i < pathPointsCnt; i++)
		{
			glm::vec3 gamePathPoint;
			Bytes >> gamePathPoint.x;
			Bytes >> gamePathPoint.y;
			Bytes >> gamePathPoint.z;

			path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(gamePathPoint)));
		}
		
		m_WoWPath = path;

		//(splineflags.cyclic)
	}
	else // linear
	{
		auto path = MakeShared(CWoWPath);

		uint32 count;
		Bytes >> count;

		glm::vec3 lastSplinePointGame;
		Bytes >> lastSplinePointGame.x;
		Bytes >> lastSplinePointGame.y;
		Bytes >> lastSplinePointGame.z;
		DestinationPoint = fromGameToReal(lastSplinePointGame);

		path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(firstSplinePointGame)));

		glm::vec3 middle = (firstSplinePointGame + lastSplinePointGame) / 2.0f;

		for (uint32 i = 1; i < count; ++i)
		{
			uint32 packedPos;
			Bytes >> packedPos;

			// this is offsets to path
			float x2 = ((packedPos      ) & 0x7FF) * 0.25f;
			float y2 = ((packedPos >> 11) & 0x7FF) * 0.25f;
			float z2 = ((packedPos >> 22) & 0x3FF) * 0.25f;

			glm::vec3 pOffs(x2, y2, z2);

			path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(middle - pOffs)));
		}

		path->AddPathNode(MakeShared(CWoWPathNode, fromGameToReal(lastSplinePointGame)));

		m_WoWPath = path;
	}

	CommitPositionAndRotation();
}

void WoWUnit::OnValuesUpdated(const UpdateMask & Mask)
{
	if (Mask.GetBit(UNIT_FIELD_DISPLAYID))
	{
		uint32 diplayID = m_Values.GetUInt32Value(UNIT_FIELD_DISPLAYID);

		if (m_HiddenNode != nullptr)
		{
			Log::Warn("WoWUnit: UNIT_FIELD_DISPLAYID updated, but Node already exists.");
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
		if (m_HiddenNode)
			m_HiddenNode->SetLocalRotationQuaternion(lookAtQuat);
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
