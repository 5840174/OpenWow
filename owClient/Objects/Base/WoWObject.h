#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWObjectValues.h"

/*enum ZN_API ObjectTypeMask : uint16
{
    TYPEMASK_OBJECT         = 0x0001,
    TYPEMASK_ITEM           = 0x0002,
    TYPEMASK_CONTAINER      = 0x0006,                       // TYPEMASK_ITEM | 0x0004
    TYPEMASK_UNIT           = 0x0008,                       // creature
    TYPEMASK_PLAYER         = 0x0010,
    TYPEMASK_GAMEOBJECT     = 0x0020,
    TYPEMASK_DYNAMICOBJECT  = 0x0040,
    TYPEMASK_CORPSE         = 0x0080,
    TYPEMASK_SEER           = TYPEMASK_PLAYER | TYPEMASK_UNIT | TYPEMASK_DYNAMICOBJECT
};*/

class ZN_API CowServerObject
	: public std::enable_shared_from_this<CowServerObject>
{
public:
	CowServerObject(CowGuid Guid);
	virtual ~CowServerObject();

	CowGuid GetWoWGUID() const { return m_GUID; }
	//uint16 GetObjectType() const { return m_ObjectType; }
	//bool IsWoWType(uint16 mask) const { return (mask & m_ObjectType) != 0; }

	void Do_UPDATETYPE_MOVEMENT(CServerPacket& Bytes);
	void Do_UPDATETYPE_VALUES(CServerPacket& Bytes);

	virtual void OnValueUpdated(uint16 index);
	virtual void OnValuesUpdated(const UpdateMask& Mask);

public: // Creation & Deletion
	virtual void Destroy();


protected:
	CowGuid m_GUID;
	CowServerObjectValues m_Values;
	//uint16 m_ObjectType;
};

#endif