#pragma once

#include "DBC__Macros.h"
// FORWARD BEGIN
class ZN_API Record;
// FORWARD END

class ZN_API DBCStats
{
	friend Record;
public:
	virtual ~DBCStats() {};

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
	const uint8* stringTable;
};

///////////////////////////////////
// Record type
///////////////////////////////////

class ZN_API Record
{
public:
	Record() : 
		m_DBC_Stats(nullptr), 
		m_Offset(nullptr) 
	{}
	Record(const Record& _record) : 
		m_DBC_Stats(_record.m_DBC_Stats), 
		m_Offset(_record.m_Offset) 
	{}
	Record(Record&& _record) = delete;
	Record(const DBCStats* _dbcStats, const uint8* offset) : 
		m_DBC_Stats(_dbcStats), 
		m_Offset(offset) 
	{}
	virtual ~Record() 
	{}

	Record& operator=(const Record& r)
	{
		m_DBC_Stats = r.m_DBC_Stats;
		m_Offset = r.m_Offset;
		return *this;
	}
	Record& operator=(Record&& r) = delete;

	const DBCStats*	getDBCStats() const { return m_DBC_Stats; }

	void            incOffset(uint32 _value) { m_Offset += _value; }
	const uint8*	getOffset() const { return m_Offset; }

	// All data has ID
	__DBC_TVALUE(uint32, ID, 1);

protected:
	// Get value with common type
	template<typename T>
	T getValue(uint32 field) const
	{
		_ASSERT_EXPR(field < m_DBC_Stats->getFieldCount(), std::to_string(field).c_str());
		return *reinterpret_cast<T*>(const_cast<uint8*>(m_Offset) + field * 4);
	}

	// Strings
	const char* getString(uint32 field) const
	{
		_ASSERT_EXPR(field < m_DBC_Stats->getFieldCount(), std::to_string(field).c_str());

		uint32_t stringOffset = getValue<uint32>(field);
		if (stringOffset >= m_DBC_Stats->getStringSize())
		{
			stringOffset = 0;
		}

		_ASSERT_EXPR(stringOffset < m_DBC_Stats->getStringSize(), std::to_string(stringOffset).c_str());
		return reinterpret_cast<char*>(const_cast<uint8*>(m_DBC_Stats->stringTable) + stringOffset);
	}

	std::string getLocalizedString(uint32 field, int8 locale = -1) const
	{
		int8 loc = locale;
		if (locale == -1)
		{
			_ASSERT_EXPR(field < m_DBC_Stats->getFieldCount() - 16, std::to_string(field).c_str());
			for (loc = 0; loc < 16; loc++)
			{
				uint32 stringOffset = getValue<uint32>(field + loc);
				if (stringOffset != 0)
				{
					break;
				}
			}
		}
		_ASSERT_EXPR(field + loc < m_DBC_Stats->getFieldCount(), std::to_string(field).c_str());
		uint32 stringOffset = getValue<uint32>(field + static_cast<uint32>(loc));

		_ASSERT_EXPR(stringOffset < m_DBC_Stats->getStringSize(), std::to_string(stringOffset).c_str());
		return Resources::Utf8_to_cp1251(reinterpret_cast<char*>(const_cast<uint8*>(m_DBC_Stats->stringTable) + stringOffset));
	}

protected:
	const DBCStats*		m_DBC_Stats;
	const uint8*		m_Offset;
};



///////////////////////////////////
// DBC File
///////////////////////////////////
template <class RECORD_T>
class DBCFile 
	: public DBCStats
{
	friend RECORD_T;
public:
	DBCFile(IFilesManager* FilesManager, const std::string& FileName);
	virtual ~DBCFile();

	// Get data by id
	inline std::shared_ptr<RECORD_T> GetRecordByID(uint32 _id) const
	{
		const auto& recordIt = m_Records.find(_id);
		if (recordIt != m_Records.end())
		{
			return recordIt->second;
		}

		return nullptr;
	}
	inline std::shared_ptr<RECORD_T> operator[](uint32 _id) const
	{
		return GetRecordByID(_id);
	}

	// Iterator that iterates over records
	class Iterator
	{
		friend RECORD_T;
	public:
		Iterator(const DBCFile* file, const uint8* offset)  
        {
            m_Record = std::make_shared<RECORD_T>(file, offset);
        }
		Iterator(Iterator& _iterator)
        {
            m_Record = std::make_shared<RECORD_T>(_iterator.m_Record);
        }

		inline Iterator& operator++()
		{
			m_Record->incOffset(m_Record->getDBCStats()->getRecordSize());
			return *this;
		}

		inline std::shared_ptr<RECORD_T> operator*() const { return m_Record; }
		inline std::shared_ptr<RECORD_T> operator->() const { return m_Record; }

		inline bool operator==(const Iterator &b) const { return m_Record->getOffset() == b.m_Record->getOffset(); }
		inline bool operator!=(const Iterator &b) const { return m_Record->getOffset() != b.m_Record->getOffset(); }

	private:
		std::shared_ptr<RECORD_T> m_Record;
	};

	inline Iterator begin() const
	{
		_ASSERT(m_File->getData() != nullptr);
		return Iterator(this, m_File->getData() + m_File->getPos());
	}

	inline Iterator end() const
	{
		_ASSERT(m_File->getData() != nullptr);
		return Iterator(this, stringTable);
	}

	inline const std::multimap<uint32, std::shared_ptr<RECORD_T>>& Records() const
	{
		return m_Records;
	}

protected:
	std::multimap<uint32, std::shared_ptr<RECORD_T>> m_Records;

private:
	std::shared_ptr<IFile> m_File;
};

#include "DBC__File.inl"