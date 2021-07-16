#pragma once

#include "UpdateMask.h"
#include "WoWObjectValuesList.h"

// FORWARD BEGIN
class CowServerObject;
// FORWARD END

class CowServerObjectValues
{
public:
	CowServerObjectValues(CowServerObject& OwnerWoWObject);
	virtual ~CowServerObjectValues();

	uint16 GetValuesCount() const;
	void SetValuesCount(uint16 ValuesCnt);
	bool IsExists(uint16 index) const;

	void Do_UPDATETYPE_VALUES(CByteBuffer& Bytes);

	int32 GetInt32Value(uint16 index) const;
	uint32 GetUInt32Value(uint16 index) const;
	uint64 GetUInt64Value(uint16 index) const;
	float GetFloatValue(uint16 index) const;
	uint8 GetByteValue(uint16 index, uint8 offset) const;
	int16 GetInt16Value(uint16 index, uint8 offset) const;
	uint16 GetUInt16Value(uint16 index, uint8 offset) const;
	CowGuid GetGuidValue(uint16 index) const;

protected:
	void PrintIndexError(uint32 index) const;

private:
	union
	{
		int32  *m_int32Values = nullptr;
		uint32 *m_uint32Values;
		float  *m_floatValues;
	};
	uint16 m_valuesCount;
	UpdateMask m_CurrentMask;
	CowServerObject& m_OwnerWoWObject;
};
