#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWObject.h"

// Additional
#include "../Units/WoWUnit.h"
#include "../GameObjects/WoWGameObject.h"


namespace
{
	enum OBJECT_UPDATE_FLAGS : uint16
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


CowServerObject::CowServerObject(CowGuid Guid)
	: m_GUID(Guid)
	, m_Values(*this)
	//, m_ObjectType(0)
{
	//m_ObjectType |= TYPEMASK_OBJECT;
	m_Values.SetValuesCount(OBJECT_END);
}

CowServerObject::~CowServerObject()
{}

void CowServerObject::Do_UPDATETYPE_MOVEMENT(CServerPacket& Bytes)
{
	CowServerWorldObject* object = dynamic_cast<CowServerWorldObject*>(this);;

	OBJECT_UPDATE_FLAGS updateFlags;
	Bytes >> updateFlags;

	if (updateFlags & UPDATEFLAG_LIVING)
	{
		if (GetWoWGUID().GetTypeId() == EWoWObjectTypeID::TYPEID_UNIT || GetWoWGUID().GetTypeId() == EWoWObjectTypeID::TYPEID_PLAYER)
		{
			CowServerUnit* unit = dynamic_cast<CowServerUnit*>(this);
			unit->ProcessMovementPacket(Bytes);
		}
	}
	else if (updateFlags & UPDATEFLAG_POSITION)
	{
		CowGuid transportGuid;
		Bytes.ReadPackedGuid(&transportGuid);
		if (object)
			object->TransportID = transportGuid;

		glm::vec3 gamePosition;
		Bytes >> gamePosition;
		if (object)
			object->Position = fromGameToReal(gamePosition);

		if (transportGuid != 0)
		{
			glm::vec3 gamePositionTransportOffset;
			Bytes >> gamePositionTransportOffset;
			if (object)
				object->PositionTransportOffset = fromGameToReal(gamePositionTransportOffset);
			//Log::Error("CowServerObject::Do_UPDATETYPE_MOVEMENT: transportGuid != 0.");
		}
		else
		{
			glm::vec3 gamePosition2;
			Bytes >> gamePosition2;
			if (object)
				object->Position = fromGameToReal(gamePosition2);
		}

		float gameOrientation;
		Bytes >> gameOrientation;
		if (object)
			object->Orientation = glm::degrees(gameOrientation + glm::half_pi<float>());

		if (GetWoWGUID().GetHigh() == EWoWObjectHighGuid::Corpse)
		{
			float gameOrientation2;
			Bytes >> gameOrientation2;
			if (object)
				object->Orientation = glm::degrees(gameOrientation2 + glm::half_pi<float>());
		}
		else
		{
			Bytes.seekRelative(4); // 0
		}
	}
	else if (updateFlags & UPDATEFLAG_STATIONARY_POSITION)
	{
		glm::vec3 gamePosition;
		Bytes >> gamePosition;
		//Log::Print("UPDATEFLAG_STATIONARY_POSITION: %f %f %f", gamePosition3.x, gamePosition3.y, gamePosition3.z);
		if (object)
			object->Position = fromGameToReal(gamePosition);

		float gameOrientation;
		Bytes >> gameOrientation;
		if (object)
			object->Orientation = glm::degrees(gameOrientation + glm::half_pi<float>());
	}

	// 0x8
	if (updateFlags & UPDATEFLAG_UNKNOWN)
	{
		Bytes.seekRelative(4);
	}

	// 0x10
	if (updateFlags & UPDATEFLAG_LOWGUID)
	{
		CowGuid::CounterType_t newCounter;
		Bytes.read(&newCounter);

		if (GetWoWGUID().GetCounter() != newCounter)
			throw CException("CowServerObject New counter '%s' isn't equal old counter '%d'.", newCounter, GetWoWGUID().GetCounter());
	}

	// 0x4
	if (updateFlags & UPDATEFLAG_HAS_TARGET)
	{
		CowGuid victimGuid;
		Bytes.ReadPackedGuid(&victimGuid); // MAYBE 0
	}

	// 0x2
	if (updateFlags & UPDATEFLAG_TRANSPORT)
	{
		CowServerGameObject* gameObject = dynamic_cast<CowServerGameObject*>(this);
		if (gameObject)
		{
			uint32 pathProgress;
			Bytes >> pathProgress;
		}
		else
		{
			Bytes.seekRelative(4);
		}
	}

	if (updateFlags & UPDATEFLAG_VEHICLE)
	{
		Bytes.seekRelative(8);
	}

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

		static const int32_t PACK_X  = 1 << 21;
		static const int32_t PACK_YZ = 1 << 20;

		static const int32_t PACK_X_MASK  = (PACK_X  << 1) - 1;
		static const int32_t PACK_YZ_MASK = (PACK_YZ << 1) - 1;

		int32_t unpackedX = (packedRotation >> 42) & PACK_X_MASK;
		int32_t unpackedY = (packedRotation >> 21) & PACK_YZ_MASK;
		int32_t unpackedZ = (packedRotation >> 0) & PACK_YZ_MASK;

		float upackedX2 = float(unpackedX) / float(PACK_X);
		if (upackedX2 > 1.0f) upackedX2 -= 2.0f;
		float upackedY2 = float(unpackedY) / float(PACK_YZ);
		if (upackedY2 > 1.0f) upackedY2 -= 2.0f;
		float upackedZ2 = float(unpackedZ) / float(PACK_YZ);
		if (upackedZ2 > 1.0f) upackedZ2 -= 2.0f;

	}

	if (object)
		object->CommitPositionAndRotation();
}

void CowServerObject::Do_UPDATETYPE_VALUES(CServerPacket& Bytes)
{
	m_Values.Do_UPDATETYPE_VALUES(Bytes);
}

void CowServerObject::OnValueUpdated(uint16 index)
{}

void CowServerObject::OnValuesUpdated(const UpdateMask & Mask)
{}



//
// Protected
//
void CowServerObject::Destroy()
{}




#endif