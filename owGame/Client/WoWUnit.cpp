#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWUnit.h"

// Additional
#include "World/WorldObjectsCreator.h"

WoWUnit::WoWUnit(IScene& Scene, ObjectGuid Guid)
	: WorldObject(Scene, Guid)
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

	PositionX = Bytes.ReadFloat();
	PositionY = Bytes.ReadFloat();
	PositionZ = Bytes.ReadFloat();
	Orientation = Bytes.ReadFloat();

	// 0x00000200
	if (HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
	{
		Bytes.ReadPackedUInt64(m_Transport.guid);
		m_Transport.pos.x = Bytes.ReadFloat();
		m_Transport.pos.y = Bytes.ReadFloat();
		m_Transport.pos.z = Bytes.ReadFloat();
		m_Transport.pos.w = Bytes.ReadFloat();
		Bytes << uint32(m_Transport.time);
		Bytes << int8(m_Transport.seat);

		if (HasExtraMovementFlag(MOVEMENTFLAG2_INTERPOLATED_MOVEMENT))
			Bytes << uint32(m_Transport.time2);
	}

	// 0x02200000
	if ((HasMovementFlag(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (HasExtraMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)))
	{
		Bytes << float(m_Pitch);
	}

	Bytes << uint32(m_FallTime);

	// 0x00001000
	if (HasMovementFlag(MOVEMENTFLAG_FALLING))
	{
		Bytes << float(m_Jump.zspeed);
		Bytes << float(m_Jump.sinAngle);
		Bytes << float(m_Jump.cosAngle);
		Bytes << float(m_Jump.xyspeed);
	}

	// 0x04000000
	if (HasMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION))
		Bytes << float(m_SplineElevation);
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
// Protected
//
std::shared_ptr<WoWUnit> WoWUnit::Create(IScene& Scene, ObjectGuid Guid)
{
	std::shared_ptr<WoWUnit> thisObj = Scene.GetRootSceneNode()->CreateSceneNode<WoWUnit>(Guid);
	Log::Green("WoWUnit created!");

	// For test only
	//BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	//bbox.calculateCenter();
	//thisObj->GetComponentT<IColliderComponent>()->SetBounds(bbox);

	return thisObj;
}

void WoWUnit::AfterCreate(IScene& Scene)
{
	uint32 displayInfo = GetUInt32Value(UNIT_FIELD_DISPLAYID);
	if (displayInfo != 0)
	{
		CWorldObjectCreator creator(Scene.GetBaseManager());
		m_HiddenNode = creator.BuildCreatureFromDisplayInfo(Scene.GetBaseManager().GetApplication().GetRenderDevice(), &Scene, displayInfo, shared_from_this());
	}
	else
	{
		_ASSERT(false);
	}
}

#endif
