#pragma once

#include "DBC__Macros.h"

class ZN_API DBCStats
{
public:
	virtual ~DBCStats() {};

	uint32_t getRecordSize() const { return recordSize; }
	uint32_t getRecordCount() const { return recordCount; }
	uint32_t getFieldCount() const { return fieldCount; }
	uint32_t getStringSize() const { return stringSize; }
	const uint8* GetStringsTable() const { return stringTable; };

protected:
	uint32_t recordSize;
	uint32_t recordCount;
	uint32_t fieldCount;
	uint32_t stringSize;
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
		_ASSERT(field < m_DBC_Stats->getFieldCount());
		return *reinterpret_cast<T*>(const_cast<uint8*>(m_Offset) + field * 4);
	}

	// Strings
	std::string getString(uint32 field) const
	{
		_ASSERT(field < m_DBC_Stats->getFieldCount());

		uint32_t stringOffset = getValue<uint32>(field);
		if (stringOffset >= m_DBC_Stats->getStringSize())
			stringOffset = 0;

		_ASSERT(stringOffset < m_DBC_Stats->getStringSize());
		return std::string(reinterpret_cast<char*>(const_cast<uint8*>(m_DBC_Stats->GetStringsTable()) + stringOffset));
	}

	std::wstring getLocalizedString(uint32 field, int8 locale = 0) const
	{
		uint32 stringOffset = getValue<uint32>(field + locale);
		if (locale == 0)
		{
			_ASSERT(field < m_DBC_Stats->getFieldCount() - 16);
			for (uint8 loc = 0; loc < 16u; loc++)
			{
				stringOffset = getValue<uint32>(field + loc);
				if (stringOffset != 0)
					break;
			}
		}

		_ASSERT(stringOffset < m_DBC_Stats->getFieldCount());
		return Resources::utf8_to_utf16(getString(field + stringOffset));
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
	inline const RECORD_T* GetRecordByID(uint32 _id) const
	{
		const auto& recordIt = m_Records.find(_id);
		if (recordIt != m_Records.end())
		{
			return &(recordIt->second);
		}

		return nullptr;
	}
	inline const RECORD_T* operator[](uint32 _id) const
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
            m_Record = RECORD_T(file, offset);
        }
		Iterator(Iterator& _iterator)
        {
            m_Record = RECORD_T(_iterator.m_Record);
        }

		inline Iterator& operator++()
		{
			m_Record.incOffset(m_Record.getDBCStats()->getRecordSize());
			return *this;
		}

		inline const RECORD_T* operator*() const { return &m_Record; }
		inline const RECORD_T* operator->() const { return &m_Record; }

		inline bool operator==(const Iterator &b) const { return m_Record.getOffset() == b.m_Record.getOffset(); }
		inline bool operator!=(const Iterator &b) const { return m_Record.getOffset() != b.m_Record.getOffset(); }

	private:
		RECORD_T m_Record;
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

	inline const std::multimap<uint32, RECORD_T>& Records() const
	{
		return m_Records;
	}

protected:
	std::multimap<uint32, RECORD_T> m_Records;

private:
	std::shared_ptr<IFile> m_File;
};

#include "DBC__File.inl"