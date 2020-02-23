#pragma once

#include "ObjectDefines.h"

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
	TYPEID_CORPSE = 7//,
	//TYPEID_AIGROUP = 8,
	//TYPEID_AREATRIGGER = 9
};

class ZN_API WoWObject
	: public SceneNode3D
{
public:

public:
	WoWObject();
	virtual ~WoWObject();

	uint64 GetGUID() const { return m_GUID; }
	uint16 GetObjectType() const { return m_ObjectType; }

	void UpdateMovement(CByteBuffer& Bytes);
	virtual void UpdateMovementData(CByteBuffer& Bytes); // For inheritance

public:
	static std::shared_ptr<WoWObject> Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene* Scene, uint64 guid);
	virtual void InitInternal(uint64 guid, uint16 ObjectType, ObjectTypeID ObjectTypeID);

protected:
	uint32 msTime;
	uint8 flags;

private:
	uint64 m_GUID;
	uint16 m_ObjectType;
	ObjectTypeID m_ObjectTypeId;
};