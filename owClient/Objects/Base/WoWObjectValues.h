#pragma once

#include "UpdateMask.h"
#include "WoWObjectValuesList.h"

// FORWARD BEGIN
class WoWObject;
// FORWARD END

class CWoWObjectValues
{
public:
	CWoWObjectValues(WoWObject& OwnerWoWObject);
	virtual ~CWoWObjectValues();

	void SetValuesCount(uint16 ValuesCnt);
	void Do_UPDATETYPE_VALUES(CByteBuffer& Bytes);

	const int32& GetInt32Value(uint16 index) const;
	const uint32& GetUInt32Value(uint16 index) const;
	const uint64& GetUInt64Value(uint16 index) const;
	const float& GetFloatValue(uint16 index) const;
	uint8 GetByteValue(uint16 index, uint8 offset) const;
	int16 GetInt16Value(uint16 index, uint8 offset) const;
	uint16 GetUInt16Value(uint16 index, uint8 offset) const;
	CWoWGuid GetGuidValue(uint16 index) const;

	void SetInt32Value(uint16 index, int32  value);
	void SetUInt32Value(uint16 index, uint32  value);
	void SetUInt64Value(uint16 index, const uint64 &value);
	void SetFloatValue(uint16 index, float   value);
	void SetByteValue(uint16 index, uint8 offset, uint8 value);
	void SetUInt16Value(uint16 index, uint8 offset, uint16 value);
	void SetInt16Value(uint16 index, uint8 offset, int16 value);
	void SetStatFloatValue(uint16 index, float value);
	void SetStatInt32Value(uint16 index, int32 value);

protected:
	bool PrintIndexError(uint32 index, bool set) const;

private:
	union
	{
		int32  *m_int32Values = nullptr;
		uint32 *m_uint32Values;
		float  *m_floatValues;
	};
	uint16 m_valuesCount;
	WoWObject& m_OwnerWoWObject;
};
