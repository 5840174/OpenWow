#include "stdafx.h"

#if 0

// General
#include "WoWObject.h"

// Additional
#include "WoWUnit.h"

WoWObject::WoWObject(ObjectGuid Guid)
	: m_GUID(Guid)
	, m_ObjectType(0)
	, m_ObjectTypeId(TYPEID_OBJECT)
{
	m_ObjectType |= TYPEMASK_OBJECT;
	m_ObjectTypeId = TYPEID_OBJECT;
	m_valuesCount = OBJECT_END;
}

WoWObject::~WoWObject()
{}

void WoWObject::ProcessMovementUpdate(CByteBuffer& Bytes)
{
	WoWUnit* unit = nullptr;
	WorldObject* object = nullptr;

	if (IsType(TYPEMASK_UNIT))
		unit = dynamic_cast<WoWUnit*>(this);
	else
		object = dynamic_cast<WorldObject*>(this);

	uint16 updateFlags;
	Bytes >> (uint16)updateFlags;

	if (updateFlags & UPDATEFLAG_LIVING)
	{
		unit->ProcessMovementPacket(Bytes);

		unit->SetSpeed(MOVE_WALK, Bytes.ReadFloat());
		unit->SetSpeed(MOVE_RUN, Bytes.ReadFloat());
		unit->SetSpeed(MOVE_RUN_BACK, Bytes.ReadFloat());
		unit->SetSpeed(MOVE_SWIM, Bytes.ReadFloat());
		unit->SetSpeed(MOVE_SWIM_BACK, Bytes.ReadFloat());
		unit->SetSpeed(MOVE_FLIGHT, Bytes.ReadFloat());
		unit->SetSpeed(MOVE_FLIGHT_BACK, Bytes.ReadFloat());
		unit->SetSpeed(MOVE_TURN_RATE, Bytes.ReadFloat());
		unit->SetSpeed(MOVE_PITCH_RATE, Bytes.ReadFloat());

		if (unit->GetMovementFlags() & MOVEMENTFLAG_SPLINE_ENABLED)
		{
			Movement::MoveSplineFlag splineFlags;
			splineFlags.raw() = Bytes.ReadUInt32();

			if (splineFlags.final_angle)
			{
				Bytes.seekRelative(4); // float
			}
			else if (splineFlags.final_target)
			{
				Bytes.seekRelative(8); // uint64
			}
			else if (splineFlags.final_point)
			{
				Bytes.seekRelative(12);
			}

			Bytes.seekRelative(12);
			//Bytes << move_spline.timePassed();
			//Bytes << move_spline.Duration();
			//Bytes << move_spline.GetId();

			Bytes.seekRelative(8);
			//Bytes << float(1.f);                             // splineInfo.duration_mod; added in 3.1
			//Bytes << float(1.f);                             // splineInfo.duration_mod_next; added in 3.1

			Bytes.seekRelative(8);
			//Bytes << move_spline.vertical_acceleration;      // added in 3.1
			//Bytes << move_spline.effect_start_time;          // added in 3.1

			uint32 nodes;
			Bytes >> nodes;
			for (size_t i = 0; i < nodes; i++)
			{
				Bytes.seekRelative(12);
			}

			Bytes.seekRelative(1 + 12);
			//Bytes << uint8(move_spline.spline.mode());       // added in 3.1
			//Bytes << (move_spline.isCyclic() ? G3D::Vector3::zero() : move_spline.FinalDestination());
		}
	}
	else
	{
		if (updateFlags & UPDATEFLAG_POSITION)
		{
			uint64 transportGuid;
			Bytes.ReadPackedUInt64(transportGuid);

			object->PositionX = Bytes.ReadFloat();
			object->PositionY = Bytes.ReadFloat();
			object->PositionZ = Bytes.ReadFloat();

			if (transportGuid != 0)
			{
				Bytes.seekRelative(12);
			}
			else
			{
				object->PositionX = Bytes.ReadFloat();
				object->PositionY = Bytes.ReadFloat();
				object->PositionZ = Bytes.ReadFloat();
			}

			object->Orientation = Bytes.ReadFloat();

			if (GetObjectTypeID() == ObjectTypeID::TYPEID_CORPSE)
			{
				object->Orientation = Bytes.ReadFloat();
			}
			else
			{
				Bytes.seekRelative(4);
			}
		}
		else
		{
			if (updateFlags & UPDATEFLAG_STATIONARY_POSITION)
			{
				object->PositionX = Bytes.ReadFloat();
				object->PositionY = Bytes.ReadFloat();
				object->PositionZ = Bytes.ReadFloat();
				object->Orientation = Bytes.ReadFloat();
			}
		}
	}

	// 0x8
	if (updateFlags & UPDATEFLAG_UNKNOWN)
	{
		Bytes.seekRelative(4);
	}

	// 0x10
	if (updateFlags & UPDATEFLAG_LOWGUID)
	{
		Bytes.seekRelative(4);
	}

	// 0x4
	if (updateFlags & UPDATEFLAG_HAS_TARGET)
	{
		uint64 victimGuid;
		Bytes.ReadPackedUInt64(victimGuid);
	}

	// 0x2
	if (updateFlags & UPDATEFLAG_TRANSPORT)
	{
		Bytes.seekRelative(4);
	}

	// 0x80
	if (updateFlags & UPDATEFLAG_VEHICLE)
	{
		Bytes.seekRelative(8);
	}

	// 0x200
	if (updateFlags & UPDATEFLAG_ROTATION)
	{
		Bytes.seekRelative(8);
	}

	if (unit)
		object = (WorldObject*)unit;

	if (object)
	{
		glm::vec3 position = fromGameToReal(glm::vec3(object->PositionX, object->PositionY, object->PositionZ));
		object->SetTranslate(position);
		object->SetRotation(glm::vec3(0.0f, object->Orientation + glm::half_pi<float>(), 0.0f));
	}
}

void WoWObject::UpdateValues(CByteBuffer& Bytes)
{
	uint8 blocksCnt;
	Bytes >> (uint8)blocksCnt;

	UpdateMask mask;
	mask.SetCount(m_valuesCount);
	Bytes.readBytes(mask.GetMask(), mask.GetLength());
	_ASSERT(blocksCnt == mask.GetBlockCount());

	m_uint32Values = new uint32[m_valuesCount];
	for (uint16 index = 0; index < m_valuesCount; index++)
		if (mask.GetBit(index))
			Bytes >> m_uint32Values[index];
}



//
// Protected
//
std::shared_ptr<WoWObject> WoWObject::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid)
{
	std::shared_ptr<WoWObject> thisObj = std::make_shared<WoWObject>(Guid);
	return thisObj;
}

void WoWObject::AfterCreate(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene)
{
}



//
// Values system
//
const int32& WoWObject::GetInt32Value(uint16 index) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	return m_int32Values[index];
}

const uint32& WoWObject::GetUInt32Value(uint16 index) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	return m_uint32Values[index];
}

const uint64& WoWObject::GetUInt64Value(uint16 index) const
{
	_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, false));
	return *((uint64*)&(m_uint32Values[index]));
}

const float& WoWObject::GetFloatValue(uint16 index) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	return m_floatValues[index];
}

uint8 WoWObject::GetByteValue(uint16 index, uint8 offset) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	_ASSERT(offset < 4);
	return *(((uint8*)&m_uint32Values[index]) + offset);
}

uint8 WoWObject::GetUInt16Value(uint16 index, uint8 offset) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	_ASSERT(offset < 2);
	return *(((uint16*)&m_uint32Values[index]) + offset);
}

void WoWObject::SetInt32Value(uint16 index, int32 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

	if (m_int32Values[index] != value)
	{
		m_int32Values[index] = value;
	}
}

void WoWObject::SetUInt32Value(uint16 index, uint32 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));
	if (m_uint32Values[index] != value)
		m_uint32Values[index] = value;
}

void WoWObject::SetUInt64Value(uint16 index, const uint64 &value)
{
	_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
	if (*((uint64*)&(m_uint32Values[index])) != value)
	{
		m_uint32Values[index] = *((uint32*)&value);
		m_uint32Values[index + 1] = *(((uint32*)&value) + 1);
	}
}

void WoWObject::SetFloatValue(uint16 index, float value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true)); //[[TZERO] : not sure of it ] _ASSERT( index < m_valuesCount || PrintIndexError( index , true ) ); 
	if (m_floatValues[index] != value)
		m_floatValues[index] = value;
}

void WoWObject::SetByteValue(uint16 index, uint8 offset, uint8 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

	if (offset > 4)
	{
		Log::Error("WoWObject::SetByteValue: wrong offset %u", offset);
		return;
	}

	if (uint8(m_uint32Values[index] >> (offset * 8)) != value)
	{
		m_uint32Values[index] &= ~uint32(uint32(0xFF) << (offset * 8));
		m_uint32Values[index] |= uint32(uint32(value) << (offset * 8));
	}
}

void WoWObject::SetUInt16Value(uint16 index, uint8 offset, uint16 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

	if (offset > 2)
	{
		Log::Error("WoWObject::SetUInt16Value: wrong offset %u", offset);
		return;
	}

	if (uint8(m_uint32Values[index] >> (offset * 16)) != value)
	{
		m_uint32Values[index] &= ~uint32(uint32(0xFFFF) << (offset * 16));
		m_uint32Values[index] |= uint32(uint32(value) << (offset * 16));
	}
}

void WoWObject::SetStatFloatValue(uint16 index, float value)
{
	if (value < 0)
		value = 0.0f;
	SetFloatValue(index, value);
}

void WoWObject::SetStatInt32Value(uint16 index, int32 value)
{
	if (value < 0)
		value = 0;
	SetUInt32Value(index, uint32(value));
}

bool WoWObject::PrintIndexError(uint32 index, bool set) const
{
	Log::Error("WoWObject: Attempt %s non-existed value field: %u (count: %u) for object typeid: %u type mask: %u", (set ? "set value to" : "get value from"), index, m_valuesCount, GetObjectTypeID(), GetObjectType());
	return true;
}

#endif