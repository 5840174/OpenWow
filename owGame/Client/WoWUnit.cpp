#include "stdafx.h"

// General
#include "WoWUnit.h"

// Additional
#include "World/WorldObjectsCreator.h"

WoWUnit::WoWUnit()
{
}

WoWUnit::~WoWUnit()
{
}

void WoWUnit::UpdateMovementData(CByteBuffer & Bytes)
{
	Bytes >> (uint32)flags2;
	Bytes.seekRelative(sizeof(uint32)); // (uint32)0xB5771D7F;

	UpdateMovementDataInternal(Bytes);

	Bytes.seekRelative(sizeof(float)); //(float)0

	Bytes >> Speed_MOVE_WALK;
	Bytes >> Speed_MOVE_RUN;
	Bytes >> Speed_MOVE_SWIM_BACK;
	Bytes >> Speed_MOVE_SWIM;
	Bytes >> Speed_MOVE_RUN_BACK;
	Bytes >> Speed_MOVE_TURN_RATE;

	if (flags2 & 0x400000)
	{
		Bytes.seekRelative(sizeof(uint32)); //(uint32)0x0;
		Bytes.seekRelative(sizeof(uint32)); //(uint32)0x659;
		Bytes.seekRelative(sizeof(uint32)); //(uint32)0xB7B;
		Bytes.seekRelative(sizeof(uint32)); //(uint32)0xFDA0B4;

		uint32 PosCount;
		Bytes >> (uint32)PosCount;
		for (int i = 0; i < PosCount + 1; i++)
			Bytes.seekRelative(3 * sizeof(float)); // (float)0;
	}
}



//
// Protected
//
std::shared_ptr<WoWUnit> WoWUnit::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid)
{
	std::shared_ptr<WoWUnit> thisObj = Scene->GetRootNode3D()->CreateSceneNode<WoWUnit>();
	thisObj->InitInternal(guid, TYPEMASK_UNIT, ObjectTypeID::TYPEID_UNIT);
	thisObj->m_valuesCount = UNIT_END;

	// For test only
	BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	bbox.calculateCenter();
	//thisObj->GetColliderComponent()->SetBounds(bbox);

	return thisObj;
}

void WoWUnit::AfterCreate(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene)
{
	uint32 displayInfo = GetUInt32Value(UNIT_FIELD_DISPLAYID);
	if (displayInfo != 0)
	{
		CWorldObjectCreator creator(BaseManager);
		m_HiddenNode = creator.BuildCreatureFromDisplayInfo(RenderDevice, Scene, displayInfo, shared_from_this());
	}
	else
	{
		_ASSERT(false);
	}
}
