#include "stdafx.h"

// General
#include "WoWObjectValues.h"

// Additional
#include "WoWObject.h"

CowServerObjectValues::CowServerObjectValues(CowServerObject & OwnerWoWObject)
	: m_OwnerWoWObject(OwnerWoWObject)
	, m_uint32Values(nullptr)
	
{}

CowServerObjectValues::~CowServerObjectValues()
{
	if (m_uint32Values != nullptr)
		delete[] m_uint32Values;
}

uint16 CowServerObjectValues::GetValuesCount() const
{
	return m_valuesCount;
}

void CowServerObjectValues::SetValuesCount(uint16 ValuesCnt)
{
	m_valuesCount = ValuesCnt;
}

bool CowServerObjectValues::IsExists(uint16 index) const
{
	return m_CurrentMask.GetBit(index);
}

void CowServerObjectValues::Do_UPDATETYPE_VALUES(CByteBuffer & Bytes)
{
	uint8 blocksCnt;
	Bytes >> blocksCnt; // each block has 32 value

	UpdateMask mask;
	mask.SetCount(m_valuesCount);
	m_CurrentMask.SetCount(m_valuesCount);

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
			m_CurrentMask.SetBit(index);

			m_OwnerWoWObject.OnValueUpdated(index);
		}
	}

	m_OwnerWoWObject.OnValuesUpdated(mask);
}


int32 CowServerObjectValues::GetInt32Value(uint16 index) const
{
	PrintIndexError(index);
	return m_int32Values[index];
}

uint32 CowServerObjectValues::GetUInt32Value(uint16 index) const
{
	PrintIndexError(index);
	return m_uint32Values[index];
}

uint64 CowServerObjectValues::GetUInt64Value(uint16 index) const
{
	PrintIndexError(index);
	return *((uint64*)&(m_uint32Values[index]));
}

float CowServerObjectValues::GetFloatValue(uint16 index) const
{
	PrintIndexError(index);
	return m_floatValues[index];
}

uint8 CowServerObjectValues::GetByteValue(uint16 index, uint8 offset) const
{
	PrintIndexError(index);
	if (offset >= 4)
		throw CException("CowServerObjectValues::GetByteValue: Offset out of bounds. %d of %d.", offset, 4);
	return *(((uint8*)&m_uint32Values[index]) + offset);
}

int16 CowServerObjectValues::GetInt16Value(uint16 index, uint8 offset) const
{
	PrintIndexError(index);
	if (offset >= 2)
		throw CException("CowServerObjectValues::GetInt16Value: Offset out of bounds. %d of %d.", offset, 2);
	return *(((int16*)&m_uint32Values[index]) + offset);
}

uint16 CowServerObjectValues::GetUInt16Value(uint16 index, uint8 offset) const
{
	PrintIndexError(index);
	if (offset >= 2)
		throw CException("CowServerObjectValues::GetUInt16Value: Offset out of bounds. %d of %d.", offset, 2);
	return *(((uint16*)&m_uint32Values[index]) + offset);
}

CowGuid CowServerObjectValues::GetGuidValue(uint16 index) const
{
	PrintIndexError(index);
	return *((CowGuid*)&(m_uint32Values[index]));
}



//
// Protected
//
void CowServerObjectValues::PrintIndexError(uint32 index) const
{
	if (index >= m_valuesCount)
		throw CException("CowServerObjectValues: Value index is out of bounds. %d of %d.", index, m_valuesCount);

	if (false == m_CurrentMask.GetBit(index))
		throw CException("CowServerObjectValues: Value with index '%d' is not setted.", index);
}
