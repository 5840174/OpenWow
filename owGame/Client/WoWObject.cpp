#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWObject.h"

// Additional
#include "WoWUnit.h"
#include "WoWGameObject.h"


namespace
{
enum OBJECT_UPDATE_FLAGS
{
	UPDATEFLAG_NONE = 0x0000,
	UPDATEFLAG_SELF = 0x0001,
	UPDATEFLAG_TRANSPORT = 0x0002,
	UPDATEFLAG_HAS_TARGET = 0x0004,
	UPDATEFLAG_UNKNOWN = 0x0008,
	UPDATEFLAG_LOWGUID = 0x0010,
	UPDATEFLAG_LIVING = 0x0020,
	UPDATEFLAG_STATIONARY_POSITION = 0x0040,
	UPDATEFLAG_VEHICLE = 0x0080,
	UPDATEFLAG_POSITION = 0x0100,
	UPDATEFLAG_ROTATION = 0x0200
};
}


WoWObject::WoWObject(CWoWObjectGuid Guid)
	: m_GUID(Guid)
	, m_Values(*this)
	, m_ObjectType(0)
{
	m_ObjectType |= TYPEMASK_OBJECT;
	m_Values.SetValuesCount(OBJECT_END);
}

WoWObject::~WoWObject()
{}

void WoWObject::ProcessMovementUpdate(CByteBuffer& Bytes)
{
	CWoWWorldObject* object = dynamic_cast<CWoWWorldObject*>(this);;

	uint16 updateFlags;
	Bytes >> updateFlags;

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
				//Bytes >> move_spline.timePassed();
				//Bytes >> move_spline.Duration();
				//Bytes >> move_spline.GetId();

				Bytes.seekRelative(8);
				//Bytes >> float(1.f);                             // splineInfo.duration_mod; added in 3.1
				//Bytes >> float(1.f);                             // splineInfo.duration_mod_next; added in 3.1

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
			object->TransportID = CWoWObjectGuid(transportGuid);

			glm::vec3 gamePosition;
			Bytes >> gamePosition.x;
			Bytes >> gamePosition.y;
			Bytes >> gamePosition.z;
			object->Position = fromGameToReal(gamePosition);

			if (transportGuid != 0)
			{
				glm::vec3 gamePositionTransportOffset;
				Bytes >> gamePositionTransportOffset.x;
				Bytes >> gamePositionTransportOffset.y;
				Bytes >> gamePositionTransportOffset.z;
				object->PositionTransportOffset = fromGameToReal(gamePositionTransportOffset);
				//Log::Error("WoWObject::ProcessMovementUpdate: transportGuid != 0.");
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

			if (GetWoWGUID().GetTypeId() == EWoWObjectTypeID::TYPEID_CORPSE)
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
				//Log::Print("UPDATEFLAG_STATIONARY_POSITION: %f %f %f", gamePosition3.x, gamePosition3.y, gamePosition3.z);
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
		//	Log::Warn("Path progress = '%d'", pathProgress);
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
	m_Values.UpdateValues(Bytes);
}

void WoWObject::OnValueUpdated(uint16 index)
{}

void WoWObject::OnValuesUpdated(const UpdateMask & Mask)
{}



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




#endif