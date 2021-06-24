#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWObject.h"

// Additional
#include "WoWUnit.h"
#include "WoWGameObject.h"

WoWObject::WoWObject(CWoWObjectGuid Guid)
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
	CWoWWorldObject* object = dynamic_cast<CWoWWorldObject*>(this);;

	if (GetWoWGUID().GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		Log::Error("asdasdasdasd");
	}

	uint16 updateFlags;
	Bytes >> (uint16)updateFlags;

	if (updateFlags & UPDATEFLAG_LIVING)
	{
		WoWUnit* unit = dynamic_cast<WoWUnit*>(this);
		if (IsWoWType(TYPEMASK_UNIT))
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

				Bytes.seekRelative(1);
				//Bytes << uint8(move_spline.spline.mode());       // added in 3.1

				Bytes.seekRelative(12);
				//Bytes << (move_spline.isCyclic() ? G3D::Vector3::zero() : move_spline.FinalDestination());
			}
		}
	}
	else
	{
		if (updateFlags & UPDATEFLAG_POSITION)
		{
			uint64 transportGuid;
			Bytes.ReadPackedUInt64(transportGuid);

			glm::vec3 gamePosition;
			Bytes >> gamePosition.x;
			Bytes >> gamePosition.y;
			Bytes >> gamePosition.z;
			object->Position = fromGameToReal(gamePosition);

			if (transportGuid != 0)
			{
				Bytes.seekRelative(12);
			}
			else
			{
				glm::vec3 gamePosition2;
				Bytes >> gamePosition2.x;
				Bytes >> gamePosition2.y;
				Bytes >> gamePosition2.z;
				object->Position = fromGameToReal(gamePosition2);
			}

			float gameOrientation;
			Bytes >> gameOrientation;
			object->Orientation = glm::degrees(gameOrientation + glm::half_pi<float>());

			if (GetObjectTypeID() == EWoWObjectTypeID::TYPEID_CORPSE)
			{
				float gameOrientation2;
				Bytes >> gameOrientation2;
				object->Orientation = glm::degrees(gameOrientation2 + glm::half_pi<float>());
			}
			else
			{
				Bytes.seekRelative(4); // 0
			}
		}
		else
		{
			if (updateFlags & UPDATEFLAG_STATIONARY_POSITION)
			{
				glm::vec3 gamePosition3;
				Bytes >> gamePosition3.x;
				Bytes >> gamePosition3.y;
				Bytes >> gamePosition3.z;
				Log::Print("UPDATEFLAG_STATIONARY_POSITION: %f %f %f", gamePosition3.x, gamePosition3.y, gamePosition3.z);
				object->Position = fromGameToReal(gamePosition3);

				float gameOrientation;
				Bytes >> gameOrientation;
				object->Orientation = glm::degrees(gameOrientation + glm::half_pi<float>());
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
		CWoWObjectGuid::CounterType_t newCounter;
		Bytes.read(&newCounter);

		if (GetWoWGUID().GetCounter() != newCounter)
			throw CException("Counter error.");
	}

	// 0x4
	if (updateFlags & UPDATEFLAG_HAS_TARGET)
	{
		uint64 victimGuid;
		Bytes.ReadPackedUInt64(victimGuid); // MAYBE 0
	}

	// 0x2
	if (updateFlags & UPDATEFLAG_TRANSPORT)
	{
		WoWGameObject* gameObject = dynamic_cast<WoWGameObject*>(this);
		if (gameObject)
		{
			uint32 pathProgress;
			Bytes >> pathProgress;
			Log::Warn("Path progress = '%d'", pathProgress);
		}
		else
		{
			Bytes.seekRelative(4);
		}
	}

	// 0x80
	if (updateFlags & UPDATEFLAG_VEHICLE)
	{
		Bytes.seekRelative(8);
	}

	// 0x200
	if (updateFlags & UPDATEFLAG_ROTATION)
	{
		/*static const int32 PACK_YZ = 1 << 20;
		static const int32 PACK_X = PACK_YZ << 1;

		static const int32 PACK_YZ_MASK = (PACK_YZ << 1) - 1;
		static const int32 PACK_X_MASK = (PACK_X << 1) - 1;

		int8 w_sign = (m_localRotation.w >= 0.f ? 1 : -1);
		int64 x = int32(m_localRotation.x * PACK_X)  * w_sign & PACK_X_MASK;
		int64 y = int32(m_localRotation.y * PACK_YZ) * w_sign & PACK_YZ_MASK;
		int64 z = int32(m_localRotation.z * PACK_YZ) * w_sign & PACK_YZ_MASK;
		m_packedRotation = z | (y << 21) | (x << 42);*/

		int64 packedRotation;
		Bytes >> packedRotation;
	}

	if (object)
		object->CommitPositionAndRotation();
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
std::shared_ptr<WoWObject> WoWObject::Create(IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWObject> thisObj = std::make_shared<WoWObject>(Guid);
	return thisObj;
}

void WoWObject::AfterCreate(IScene& Scene)
{
}

void WoWObject::Destroy()
{}



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

int16 WoWObject::GetInt16Value(uint16 index, uint8 offset) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	_ASSERT(offset < 2);
	return *(((int16*)&m_uint32Values[index]) + offset);
}

uint16 WoWObject::GetUInt16Value(uint16 index, uint8 offset) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	_ASSERT(offset < 2);
	return *(((uint16*)&m_uint32Values[index]) + offset);
}

CWoWObjectGuid WoWObject::GetGuidValue(uint16 index) const
{
	_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, false));
	return *((CWoWObjectGuid*)&(m_uint32Values[index]));
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
		throw CException("WoWObject::SetByteValue: wrong offset %u", offset);

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
		throw CException("WoWObject::SetUInt16Value: wrong offset %u", offset);

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