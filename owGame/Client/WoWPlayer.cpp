#include "stdafx.h"

// General
#include "WoWPlayer.h"

WoWPlayer::WoWPlayer()
{
}

WoWPlayer::~WoWPlayer()
{
}

void WoWPlayer::UpdateMovementData(CByteBuffer & Bytes)
{
	Bytes >> (uint32)flags2;
	Bytes >> (uint32)msTime;

	if ((flags2 & 0x02000000) == 0) // GetTransport() == nullptr
	{
		UpdateMovementDataInternal(Bytes);
	}
	else
	{
		Bytes >> transportX;
		Bytes >> transportY;
		Bytes >> transportZ;
		Bytes >> transportO;
		Bytes >> transportGUID;

		UpdateMovementDataInternal(Bytes);
	}

	Bytes.seekRelative(sizeof(float)); // (float)0;

	if (flags2 & 0x2000) // update self
	{
		Bytes.seekRelative(4 * sizeof(float));
		//Bytes >> (float)0;
		//Bytes >> (float)1.0;
		//Bytes >> (float)0;
		//Bytes >> (float)0;
	}

	Bytes >> Speed_MOVE_WALK;
	Bytes >> Speed_MOVE_RUN;
	Bytes >> Speed_MOVE_SWIM_BACK;
	Bytes >> Speed_MOVE_SWIM;
	Bytes >> Speed_MOVE_RUN_BACK;
	Bytes >> Speed_MOVE_TURN_RATE;
}



//
// Protected
//
std::shared_ptr<WoWPlayer> WoWPlayer::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid)
{
	std::shared_ptr<WoWPlayer> thisObj = Scene->GetRootNode3D()->CreateSceneNode<WoWPlayer>();
	thisObj->InitInternal(guid, TYPEMASK_PLAYER, ObjectTypeID::TYPEID_PLAYER);
	thisObj->m_valuesCount = PLAYER_END;

	// For test only
	BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	bbox.calculateCenter();
	//thisObj->GetComponent<IColliderComponent3D>()->SetBounds(bbox);

	return thisObj;
}
