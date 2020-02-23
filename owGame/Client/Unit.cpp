#include "stdafx.h"

// General
#include "Unit.h"

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
	Bytes >> PositionX;
	Bytes >> PositionY;
	Bytes >> PositionZ;
	Bytes >> Orientation;

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


	if (m_HiddenNode)
	{
		glm::vec3 position = fromGameToReal(glm::vec3(PositionX, PositionY, PositionZ));
		SetTranslate(position);
		SetRotation(glm::vec3(0.0f, Orientation, 0.0f));
	}
}



//
// Protected
//
std::shared_ptr<WoWUnit> WoWUnit::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid)
{
	std::shared_ptr<WoWUnit> thisObj = Scene->GetRootNode3D()->CreateSceneNode<WoWUnit>();
	thisObj->InitInternal(guid, TYPEMASK_UNIT, ObjectTypeID::TYPEID_UNIT);

	CWorldObjectCreator creator(BaseManager);
	thisObj->m_HiddenNode = creator.BuildCreatureFromDisplayInfo(RenderDevice, Scene, 6910, thisObj);

	return thisObj;
}
