#pragma once

// Record create
#define CONCAT_GET(a) Get_##a
#define CONCAT_RECORD(a) a##Record

// Add data

#define __DBC_TVALUE(type, _name, _field)                    \
type CONCAT_GET(_name)() const                               \
{                                                            \
	return getValue<type>(static_cast<uint32>(_field - 1));  \
}

#define __DBC_TARRAY(_type, _name, _field, _size)                      \
_type CONCAT_GET(_name)(uint8 _index) const                            \
{                                                                      \
    _ASSERT(_index < _size);                                           \
	return getValue<_type>(static_cast<uint32>(_field - 1 + _index));  \
}

#define __DBC_STRING(_name, _field)                     \
const char* CONCAT_GET(_name)() const                   \
{                                                       \
	return getString(static_cast<uint32>(_field - 1));  \
}

#define __DBC_STRARR(_name, _field, _size)                       \
const char* CONCAT_GET(_name)(uint8 _index) const                \
{                                                                \
	_ASSERT(_index < static_cast<size_t>(_size));                                     \
	return getString(static_cast<uint32>(_field - 1 + _index));  \
}

#define __DBC_LOCSTR(_name, _field)                              \
std::string CONCAT_GET(_name)(int8 _locale = -1) const           \
{                                                                \
	return getLocalizedString(static_cast<uint32>(_field - 1));  \
}

#define __DBC_REF_ID(_dbc, _name, _field)                                                       \
class TEST##_dbc;                                                                                   \
const uint32/*std::shared_ptr<CONCAT_RECORD(_dbc)>*/ CONCAT_GET(_name)() const                             \
{                                                                                         \
	return /*getDBCStats()->GetDBCsStorage()->_dbc().GetRecordByID(*/getValue<uint32>(static_cast<uint32>(_field - 1))/*)*/;  \
}

#define __DBC_REFSID(_dbc, _name, _field, _size)                                                   \
class TEST##_dbc;                                                                                      \
const uint32/*std::shared_ptr<_dbc##Record>*/ CONCAT_GET(_name)(uint8 _index) const                          \
{                                                                                                  \
	return /*_dbc[static_cast<uint32>(*/getValue<uint32>(static_cast<uint32>(_field - 1 + _index))/*)]*/;  \
}

// Uses in common classes

#define FOREIGN_KEY(_type, _dbc, _refFieldName, _dispName)                                                   \
_type __##_dispName;                                                                                         \
const CONCAT_RECORD(_dbc)* _dispName(const CDBCStorage* Storage) const                                       \
{                                                                                                            \
	for (const auto& it : Storage->##_dbc())								                                 \
	{                                                                                                        \
		if (static_cast<_type>(it->CONCAT_GET(_refFieldName)()) == __##_dispName)                            \
		{                                                                                                    \
			return it;                                                                                       \
		}                                                                                                    \
	}                                                                                                        \
                                                                                                             \
	return nullptr;                                                                                          \
}

#define FOREIGN_KEY_ID(_type, _dbc, _dispName)                                                               \
_type __##_dispName;                                                                                         \
const CONCAT_RECORD(_dbc)* _dispName(const CDBCStorage* Storage) const                                       \
{                                                                                                            \
    return Storage->##_dbc().GetRecordByID(static_cast<uint32>(__##_dispName));                              \
}

//------------------------------------------
// Create class
//------------------------------------------

// Class begin
#define DBC_DEF_BEGIN(accessName)                                                                     \
class CONCAT_RECORD(accessName) : public Record {                                                     \
public:                                                                                               \
	CONCAT_RECORD(accessName)() {}                                                                    \
	CONCAT_RECORD(accessName)(const DBCFile<CONCAT_RECORD(accessName)>::Iterator& _iterator) :        \
		Record(_iterator->m_DBC_Stats, _iterator->m_Offset)                                           \
	{}                                                                                                \
	CONCAT_RECORD(accessName)(const DBCFile<CONCAT_RECORD(accessName)>* file, const uint8* offset) :  \
		Record(file, offset)                                                                          \
	{}                                                                                                \
	virtual ~##CONCAT_RECORD(accessName)##() {}                                                              \
public:                                                                                                      \


// Class end
#define DBC_DEF_END                                                                                          \
};


// Placed in *.cpp files
#define DBC_LOAD(accessName, filesManagerObject, fileName) m_##accessName = std::make_unique<DBCFile<CONCAT_RECORD(accessName)>>(filesManagerObject, fileName);

// Placed in *.h files
#define DBC_DEFINE(accessName)                                                                              \
public:                                                                                                     \
const DBCFile<CONCAT_RECORD(accessName)>& accessName() const { return *(m_##accessName.get()); }            \
private:                                                                                                    \
std::unique_ptr<DBCFile<CONCAT_RECORD(accessName)>> m_##accessName;
