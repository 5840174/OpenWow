#include "stdafx.h"

// General
#include "GameObject.h"

WoWGameObject::WoWGameObject()
{
}

WoWGameObject::~WoWGameObject()
{
}



//
// Protected
//
std::shared_ptr<WoWGameObject> WoWGameObject::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, uint64 guid)
{
	std::shared_ptr<WoWGameObject> thisObj = Scene->GetRootNode3D()->CreateSceneNode<WoWGameObject>();
	thisObj->InitInternal(guid, TYPEMASK_GAMEOBJECT, ObjectTypeID::TYPEID_GAMEOBJECT);
	return thisObj;
}
