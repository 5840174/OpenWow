#pragma once

#include "ObjectDefines.h"
#include "UpdateMask.h"
#include "UpdateFields.h"

enum ZN_API ObjectTypeMask : uint16
{
	TYPEMASK_OBJECT = 0x0001,
	TYPEMASK_ITEM = 0x0002,
	TYPEMASK_CONTAINER = 0x0006,
	TYPEMASK_UNIT = 0x0008,
	TYPEMASK_PLAYER = 0x0010,
	TYPEMASK_GAMEOBJECT = 0x0020,
	TYPEMASK_DYNAMICOBJECT = 0x0040,
	TYPEMASK_CORPSE = 0x0080,
	TYPEMASK_AIGROUP = 0x0100,
	TYPEMASK_AREATRIGGER = 0x0200
};


enum class ZN_API ObjectTypeID : uint8
{
	TYPEID_OBJECT = 0,        // WoWObject
	TYPEID_ITEM = 1,
	TYPEID_CONTAINER = 2,
	TYPEID_UNIT = 3,          // WoWUnit
	TYPEID_PLAYER = 4,        // WoWPlayer
	TYPEID_GAMEOBJECT = 5,    // WoWGameObject
	TYPEID_DYNAMICOBJECT = 6,
	TYPEID_CORPSE = 7
};

enum class ZN_API OBJECT_UPDATE_TYPE : uint8
{
	UPDATETYPE_VALUES = 0,
	UPDATETYPE_MOVEMENT = 1,
	UPDATETYPE_CREATE_OBJECT = 2,
	UPDATETYPE_CREATE_OBJECT2 = 3,
	UPDATETYPE_OUT_OF_RANGE_OBJECTS = 4,
	UPDATETYPE_NEAR_OBJECTS = 5
};

enum OBJECT_UPDATE_FLAGS
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


// TODO: MOVE ME!!!
enum CreatureFlagsExtra
{
	CREATURE_FLAG_EXTRA_INSTANCE_BIND = 0x00000001,       // creature kill bind instance with killer and killer's group
	CREATURE_FLAG_EXTRA_CIVILIAN = 0x00000002,       // not aggro (ignore faction/reputation hostility)
	CREATURE_FLAG_EXTRA_NO_PARRY = 0x00000004,       // creature can't parry
	CREATURE_FLAG_EXTRA_NO_PARRY_HASTEN = 0x00000008,       // creature can't counter-attack at parry
	CREATURE_FLAG_EXTRA_NO_BLOCK = 0x00000010,       // creature can't block
	CREATURE_FLAG_EXTRA_NO_CRUSH = 0x00000020,       // creature can't do crush attacks
	CREATURE_FLAG_EXTRA_NO_XP_AT_KILL = 0x00000040,       // creature kill not provide XP
	CREATURE_FLAG_EXTRA_TRIGGER = 0x00000080,       // trigger creature
	CREATURE_FLAG_EXTRA_WORLDEVENT = 0x00004000,       // custom flag for world event creatures (left room for merging)
	//CREATURE_FLAG_EXTRA_CHARM_AI        = 0x00008000,       // use ai when charmed
	CREATURE_FLAG_EXTRA_NO_TAUNT = 0x00010000,       // cannot be taunted
	CREATURE_FLAG_EXTRA_NO_CRIT = 0x00020000,       // creature can't do critical strikes
};

inline ObjectTypeID GuidHigh2TypeId(uint32 guid_hi)
{
	switch (guid_hi)
	{
		case HIGHGUID_ITEM:         
			return ObjectTypeID::TYPEID_ITEM;
		//case HIGHGUID_CONTAINER:    
		//	return TYPEID_CONTAINER; HIGHGUID_CONTAINER==HIGHGUID_ITEM currently
		case HIGHGUID_UNIT:         
			return ObjectTypeID::TYPEID_UNIT;
		case HIGHGUID_PET:          
			return ObjectTypeID::TYPEID_UNIT;
		case HIGHGUID_PLAYER:       
			return ObjectTypeID::TYPEID_PLAYER;
		case HIGHGUID_GAMEOBJECT:   
			return ObjectTypeID::TYPEID_GAMEOBJECT;
		case HIGHGUID_DYNAMICOBJECT:
			return ObjectTypeID::TYPEID_DYNAMICOBJECT;
		case HIGHGUID_CORPSE:       
			return ObjectTypeID::TYPEID_CORPSE;
		case HIGHGUID_MO_TRANSPORT: 
			return ObjectTypeID::TYPEID_GAMEOBJECT;
	}
	return ObjectTypeID::TYPEID_OBJECT;                                              // unknown
}

class ZN_API WoWObject
{
public:

public:
	WoWObject();
	virtual ~WoWObject();

	uint64 GetGUID() const { return m_GUID; }
	uint16 GetObjectType() const { return m_ObjectType; }
	ObjectTypeID GetObjectTypeID() const { return m_ObjectTypeId; }

	void UpdateMovement(CByteBuffer& Bytes);
	void UpdateValues(CByteBuffer& Bytes);
	virtual void UpdateMovementData(CByteBuffer& Bytes);


public: // Creation
	static std::shared_ptr<WoWObject> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene* Scene, uint64 guid);
	virtual void InitInternal(uint64 guid, uint16 ObjectType, ObjectTypeID ObjectTypeID);
	virtual void AfterCreate(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene);

public: // Values system
	const int32& GetInt32Value(uint16 index) const;
	const uint32& GetUInt32Value(uint16 index) const;
	const uint64& GetUInt64Value(uint16 index) const;
	const float& GetFloatValue(uint16 index) const;
	uint8 GetByteValue(uint16 index, uint8 offset) const;
	uint8 GetUInt16Value(uint16 index, uint8 offset) const;
	void SetInt32Value(uint16 index, int32  value);
	void SetUInt32Value(uint16 index, uint32  value);
	void SetUInt64Value(uint16 index, const uint64 &value);
	void SetFloatValue(uint16 index, float   value);
	void SetByteValue(uint16 index, uint8 offset, uint8 value);
	void SetUInt16Value(uint16 index, uint8 offset, uint16 value);
	void SetInt16Value(uint16 index, uint8 offset, int16 value) { SetUInt16Value(index, offset, (uint16)value); }
	void SetStatFloatValue(uint16 index, float value);
	void SetStatInt32Value(uint16 index, int32 value);
	bool PrintIndexError(uint32 index, bool set) const;
	union
	{
		int32  *m_int32Values;
		uint32 *m_uint32Values;
		float  *m_floatValues;
	};
	uint16 m_valuesCount;


protected:
	uint32 msTime;
	uint8 flags;

private:
	uint64 m_GUID;
	uint16 m_ObjectType;
	ObjectTypeID m_ObjectTypeId;
};