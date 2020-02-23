#include "stdafx.h"

// General
#include "Object.h"

WoWObject::WoWObject()
	: m_GUID(0)
	, m_ObjectType(ObjectTypeMask::TYPEMASK_OBJECT)
{}

WoWObject::~WoWObject()
{}

void WoWObject::UpdateMovement(CByteBuffer & Bytes)
{
	Bytes >> (uint8)flags;

	// INHERITANCE
	UpdateMovementData(Bytes);
	// INHERITANCE

	uint32 unk0;
	Bytes.read(&unk0); //(uint32)0x1;
	_ASSERT(unk0 == 0x1);

	if (GUID_HIPART(m_GUID) == HIGHGUID_TRANSPORT)
	{
		Bytes >> (uint32)msTime;
	}

	if (GUID_HIPART(m_GUID) == HIGHGUID_PLAYER_CORPSE)
	{
		uint32 unk1;
		Bytes.read(&unk1); //(uint32)0xBD38BA14;
		_ASSERT(unk1 == 0xBD38BA14);
	}
}

void WoWObject::UpdateMovementData(CByteBuffer & Bytes)
{
	// Do nothing
}



//
// Protected
//
std::shared_ptr<WoWObject> WoWObject::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid)
{
	std::shared_ptr<WoWObject> thisObj = Scene->GetRootNode3D()->CreateSceneNode<WoWObject>();
	thisObj->InitInternal(guid, TYPEMASK_OBJECT, ObjectTypeID::TYPEID_OBJECT);

	return thisObj;
}

void WoWObject::InitInternal(uint64 guid, uint16 ObjectType, ObjectTypeID ObjectTypeID)
{
	m_GUID = guid;
	m_ObjectType = ObjectType;
	m_ObjectTypeId = ObjectTypeID;

	BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	bbox.calculateCenter();
	GetComponent<IColliderComponent3D>()->SetBounds(bbox);
}
