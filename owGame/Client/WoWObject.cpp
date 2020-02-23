#include "stdafx.h"

// General
#include "WoWObject.h"

// Additional
#include "WoWUnit.h"

WoWObject::WoWObject()
	: m_GUID(0)
	, m_ObjectType(ObjectTypeMask::TYPEMASK_OBJECT)
{}

WoWObject::~WoWObject()
{}

void WoWObject::UpdateMovement(CByteBuffer & Bytes)
{
	Bytes >> (uint8)flags;

	// INHERITANCE
	UpdateMovementData(Bytes);
	// INHERITANCE

	uint32 unk0;
	Bytes.read(&unk0); //(uint32)0x1;
	_ASSERT(unk0 == 0x1);

	if (GUID_HIPART(m_GUID) == HIGHGUID_TRANSPORT)
	{
		Bytes >> (uint32)msTime;
	}

	if (GUID_HIPART(m_GUID) == HIGHGUID_PLAYER_CORPSE)
	{
		uint32 unk1;
		Bytes.read(&unk1); //(uint32)0xBD38BA14;
		_ASSERT(unk1 == 0xBD38BA14);
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

void WoWObject::UpdateMovementData(CByteBuffer & Bytes)
{
	// Do nothing
}



//
// Protected
//
std::shared_ptr<WoWObject> WoWObject::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid)
{
	std::shared_ptr<WoWObject> thisObj = std::make_shared<WoWObject>();
	thisObj->InitInternal(guid, TYPEMASK_OBJECT, ObjectTypeID::TYPEID_OBJECT);
	thisObj->m_valuesCount = OBJECT_END;
	return thisObj;
}

void WoWObject::InitInternal(uint64 guid, uint16 ObjectType, ObjectTypeID ObjectTypeID)
{
	m_GUID = guid;
	m_ObjectType = ObjectType;
	m_ObjectTypeId = ObjectTypeID;
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
