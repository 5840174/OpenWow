#include "stdafx.h"

// General
#include "WoWObjectValues.h"

// Additional
#include "WoWObject.h"

CWoWObjectValues::CWoWObjectValues(WoWObject & OwnerWoWObject)
	: m_OwnerWoWObject(OwnerWoWObject)
	, m_uint32Values(nullptr)
	
{}

CWoWObjectValues::~CWoWObjectValues()
{
	if (m_uint32Values != nullptr)
		delete[] m_uint32Values;
}

void CWoWObjectValues::SetValuesCount(uint16 ValuesCnt)
{
	m_valuesCount = ValuesCnt;
}

void CWoWObjectValues::UpdateValues(CByteBuffer & Bytes)
{
	uint8 blocksCnt;
	Bytes >> blocksCnt; // each block has 32 value

	UpdateMask mask;
	mask.SetCount(m_valuesCount);

	Bytes.readBytes(mask.GetMask(), mask.GetMaskLengthBytes());

	if (blocksCnt != mask.GetBlockCount())
		throw CException("Blocks count mismatch.");

	if (m_uint32Values != nullptr)
		delete[] m_uint32Values;

	m_uint32Values = new uint32[m_valuesCount];
	for (uint16 index = 0; index < m_valuesCount; index++)
	{
		if (mask.GetBit(index))
		{
			Bytes >> m_uint32Values[index];
			m_OwnerWoWObject.OnValueUpdated(index);
		}
	}

	m_OwnerWoWObject.OnValuesUpdated(mask);
}


const int32& CWoWObjectValues::GetInt32Value(uint16 index) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	return m_int32Values[index];
}

const uint32& CWoWObjectValues::GetUInt32Value(uint16 index) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	return m_uint32Values[index];
}

const uint64& CWoWObjectValues::GetUInt64Value(uint16 index) const
{
	_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, false));
	return *((uint64*)&(m_uint32Values[index]));
}

const float& CWoWObjectValues::GetFloatValue(uint16 index) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	return m_floatValues[index];
}

uint8 CWoWObjectValues::GetByteValue(uint16 index, uint8 offset) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	_ASSERT(offset < 4);
	return *(((uint8*)&m_uint32Values[index]) + offset);
}

int16 CWoWObjectValues::GetInt16Value(uint16 index, uint8 offset) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	_ASSERT(offset < 2);
	return *(((int16*)&m_uint32Values[index]) + offset);
}

uint16 CWoWObjectValues::GetUInt16Value(uint16 index, uint8 offset) const
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, false));
	_ASSERT(offset < 2);
	return *(((uint16*)&m_uint32Values[index]) + offset);
}

CWoWGuid CWoWObjectValues::GetGuidValue(uint16 index) const
{
	_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, false));
	return *((CWoWGuid*)&(m_uint32Values[index]));
}

void CWoWObjectValues::SetInt32Value(uint16 index, int32 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

	if (m_int32Values[index] != value)
	{
		m_int32Values[index] = value;
	}
}

void CWoWObjectValues::SetUInt32Value(uint16 index, uint32 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));
	if (m_uint32Values[index] != value)
		m_uint32Values[index] = value;
}

void CWoWObjectValues::SetUInt64Value(uint16 index, const uint64 &value)
{
	_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
	if (*((uint64*)&(m_uint32Values[index])) != value)
	{
		m_uint32Values[index] = *((uint32*)&value);
		m_uint32Values[index + 1] = *(((uint32*)&value) + 1);
	}
}

void CWoWObjectValues::SetFloatValue(uint16 index, float value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true)); //[[TZERO] : not sure of it ] _ASSERT( index < m_valuesCount || PrintIndexError( index , true ) ); 
	if (m_floatValues[index] != value)
		m_floatValues[index] = value;
}

void CWoWObjectValues::SetByteValue(uint16 index, uint8 offset, uint8 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

	if (offset > 4)
		throw CException("CWoWObjectValues::SetByteValue: wrong offset %u", offset);

	if (uint8(m_uint32Values[index] >> (offset * 8)) != value)
	{
		m_uint32Values[index] &= ~uint32(uint32(0xFF) << (offset * 8));
		m_uint32Values[index] |= uint32(uint32(value) << (offset * 8));
	}
}

void CWoWObjectValues::SetUInt16Value(uint16 index, uint8 offset, uint16 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

	if (offset > 2)
		throw CException("CWoWObjectValues::SetUInt16Value: wrong offset %u", offset);

	if (uint8(m_uint32Values[index] >> (offset * 16)) != value)
	{
		m_uint32Values[index] &= ~uint32(uint32(0xFFFF) << (offset * 16));
		m_uint32Values[index] |= uint32(uint32(value) << (offset * 16));
	}
}

void CWoWObjectValues::SetInt16Value(uint16 index, uint8 offset, int16 value)
{
	_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

	if (offset > 2)
		throw CException("CWoWObjectValues::SetInt16Value: wrong offset %u", offset);

	if (uint8(m_uint32Values[index] >> (offset * 16)) != value)
	{
		m_uint32Values[index] &= ~uint32(uint32(0xFFFF) << (offset * 16));
		m_uint32Values[index] |= uint32(uint32(value) << (offset * 16));
	}
}

void CWoWObjectValues::SetStatFloatValue(uint16 index, float value)
{
	if (value < 0)
		value = 0.0f;
	SetFloatValue(index, value);
}

void CWoWObjectValues::SetStatInt32Value(uint16 index, int32 value)
{
	if (value < 0)
		value = 0;
	SetUInt32Value(index, uint32(value));
}

bool CWoWObjectValues::PrintIndexError(uint32 index, bool set) const
{
	//Log::Error("CWoWObjectValues: Attempt %s non-existed value field: %u (count: %u) for object typeid: %u type mask: %u", (set ? "set value to" : "get value from"), index, m_valuesCount, m_OwnerWoWObject.GetObjectTypeID(), GetObjectType());
	return true;
}
