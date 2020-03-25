#include "stdafx.h"

// General
#include "WoWPlayer.h"

WoWPlayer::WoWPlayer(ObjectGuid Guid)
	: WoWUnit(Guid)
{
	m_ObjectType |= TYPEMASK_PLAYER;
	m_ObjectTypeId = TYPEID_PLAYER;
	m_valuesCount = PLAYER_END;
}

WoWPlayer::~WoWPlayer()
{
}



//
// Protected
//
std::shared_ptr<WoWPlayer> WoWPlayer::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid)
{
	std::shared_ptr<WoWPlayer> thisObj = Scene->GetRootNode3D()->CreateSceneNode<WoWPlayer>(Guid);

	// For test only
	BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	bbox.calculateCenter();
	//thisObj->GetColliderComponent()->SetBounds(bbox);

	return thisObj;
}
