#pragma once

// Record create
#define CONCAT_GET(a) Get_##a
#define CONCAT_CLASS(a) a##Record

// Add data

#define __DBC_TVALUE(type, _name, _field) \
type CONCAT_GET(_name)() const \
{ \
	return getValue<type>(static_cast<uint32>(_field - 1)); \
}

#define __DBC_STRING(_name, _field) \
const char* CONCAT_GET(_name)() const \
{ \
	return getString(static_cast<uint32>(_field - 1)); \
}

#define __DBC_LOCSTR(_name, _field) \
const char* CONCAT_GET(_name)(int8 _locale = -1) const \
{ \
	return getString(static_cast<uint32>(_field - 1)); \
}

#define __DBC_REF_ID(_dbc, _name, _field) \
_dbc##Record* CONCAT_GET(_name)() const \
{ \
	return _dbc[static_cast<uint32>(getValue<uint32>(static_cast<uint32>(_field - 1)))]; \
}

#define __DBC_TARRAY(_type, _name, _field, _size) \
_type CONCAT_GET(_name)(uint8 _index) const \
{ \
    assert1(_index < _size); \
	return getValue<_type>(static_cast<uint32>(_field - 1 + _index)); \
}

// Uses in common classes

#define __DBC_FOREIGN_KEY(_type, _dbc, _refFieldName, _dispName) \
_type __##_dispName;\
const CONCAT_CLASS(_dbc)* _dispName() \
{ \
	for (auto it = _dbc.begin(); it != _dbc.end(); ++it) \
	{ \
		if (static_cast<_type>(it->CONCAT_GET(_refFieldName)()) == __##_dispName) \
		{ \
			return &(*it); \
		} \
	} \
 \
	return nullptr; \
}

#define __DBC_FOREIGN_KEY_ID(_type, _dbc, _dispName) \
_type __##_dispName;\
const _dbc##Record* _dispName() \
{ \
    return _dbc[static_cast<uint32>(__##_dispName)]; \
}


// Create class

#define DBC_DEF_BEGIN(accessName) \
class CONCAT_CLASS(accessName) : public Record { \
public: \
	CONCAT_CLASS(accessName)(const DBCFile<CONCAT_CLASS(accessName)>::Iterator& _iterator) : Record(_iterator->dbcStats, _iterator->offset) { } \
	CONCAT_CLASS(accessName)(DBCFile<CONCAT_CLASS(accessName)>* file, uint8* offset) : Record(file, offset) { } \
public:

#define DBC_DEF_END \
};

// Create accessor

#define DBC_DEFINE(accessName) \
class CONCAT_CLASS(accessName); \
extern DBCFile<CONCAT_CLASS(accessName)> accessName;

#define DBC_LOAD(accessName, fileName) DBCFile<CONCAT_CLASS(accessName)> accessName(fileName);



///////////////////////////////////
// DBC STATS
///////////////////////////////////
class Record;
class DBCStats
{
public:
	uint32_t getRecordSize() const { return recordSize; }
	uint32_t getRecordCount() const { return recordCount; }
	uint32_t getFieldCount() const { return fieldCount; }
	uint32_t getStringSize() const { return stringSize; }

protected:
	uint32_t recordSize;
	uint32_t recordCount;
	uint32_t fieldCount;
	uint32_t stringSize;

	// Strings
	uint8* stringTable;

	//

	friend Record;
};



///////////////////////////////////
// Record type
///////////////////////////////////
class Record
{
public:
	Record() = delete;
	Record(const Record& _record) = delete;
	Record(DBCStats* _dbcStats, uint8* offset) : dbcStats(_dbcStats), offset(offset) {}

	Record& operator=(const Record& r) = delete;

	// All data has ID
	__DBC_TVALUE(uint32, ID, 1);

protected:
	// Get value with common type
	template<typename T>
	T getValue(uint32 field) const
	{
		assert2(field < dbcStats->fieldCount, std::to_string(field).c_str());
		return *reinterpret_cast<T*>(offset + field * 4);
	}

	// Strings
	const char* getString(uint32 field) const
	{
		assert2(field < dbcStats->fieldCount, std::to_string(field).c_str());

		uint32_t stringOffset = getValue<uint32>(field);
		if (stringOffset >= dbcStats->stringSize)
		{
			stringOffset = 0;
		}

		assert2(stringOffset < dbcStats->stringSize, std::to_string(stringOffset).c_str());
		return reinterpret_cast<char*>(dbcStats->stringTable + stringOffset);
	}
	const char* getLocalizedString(uint32 field, int8 locale = -1) const
	{
		int8 loc = locale;
		if (locale == -1)
		{
			assert2(field < dbcStats->fieldCount - 8, std::to_string(field).c_str());
			for (loc = 0; loc < 8; loc++)
			{
				uint32 stringOffset = getValue<uint32>(field + loc);
				if (stringOffset != 0)
				{
					break;
				}
			}
		}
		assert2(field + loc < dbcStats->fieldCount, std::to_string(field).c_str());
		uint32 stringOffset = getValue<uint32>(field + static_cast<uint32>(loc));

		assert2(stringOffset < dbcStats->stringSize, std::to_string(stringOffset).c_str());
		return reinterpret_cast<char*>(dbcStats->stringTable + stringOffset);
	}

public:
	DBCStats* dbcStats;
	uint8* offset;
};



///////////////////////////////////
// DBC File
///////////////////////////////////
class File;
template <class RECORD_T>
class DBCFile : public File, public DBCStats
{
	friend RECORD_T;

public:
	DBCFile(cstring _file) : File(string("DBFilesClient\\") + string(_file))
	{}

	~DBCFile()
	{
		for (auto it = records.begin(); it != records.end();)
		{
			auto obj = it->second;
			it = records.erase(it);
			delete obj;
		}
	}

	// Open file and fill data
	bool Open();

	// Get data by id
	RECORD_T* operator[](uint32 _id);

	///////////////////////////////////
	// Iterator that iterates over records
	///////////////////////////////////
	class Iterator
	{
	public:
		Iterator(DBCFile* file, uint8* offset) : record(file, offset) {}
		Iterator(Iterator& _iterator) : record(_iterator.record) {}

		Iterator& operator++()
		{
			record.offset += record.dbcStats->getRecordSize();
			return *this;
		}

		const RECORD_T& operator*() const
		{
			return record;
		}
		const RECORD_T* operator->() const
		{
			return &record;
		}

		bool operator==(const Iterator &b) const
		{
			return record.offset == b.record.offset;
		}
		bool operator!=(const Iterator &b) const
		{
			return record.offset != b.record.offset;
		}

	private:
		RECORD_T record;

		//
		friend RECORD_T;
	};

	// Iterators
	Iterator begin()
	{
		assert1(GetData() != nullptr);
		return Iterator(this, data + GetPos());
	}

	Iterator end()
	{
		assert1(GetData() != nullptr);
		return Iterator(this, stringTable);
	}

	const map<uint32, RECORD_T*>* Records() const
	{
		return &records;
	}

protected:
	map<uint32, RECORD_T*> records;
};

#include "DBC__File.inl"