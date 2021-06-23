#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWPlayer.h"

WoWPlayer::WoWPlayer(IScene& Scene, CWoWObjectGuid Guid)
	: WoWUnit(Scene, Guid)
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
std::shared_ptr<WoWPlayer> WoWPlayer::Create(IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWPlayer> thisObj = Scene.GetRootSceneNode()->CreateSceneNode<WoWPlayer>(Guid);

	// For test only
	//BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	//bbox.calculateCenter();
	//thisObj->GetComponentT<IColliderComponent>()->SetBounds(bbox);

	return thisObj;
}

#endif
