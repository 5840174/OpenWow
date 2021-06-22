#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWGameObject.h"

// Additional
#include "World/WorldObjectsCreator.h"

WoWGameObject::WoWGameObject(IScene& Scene, ObjectGuid Guid)
	: WorldObject(Scene, Guid)
{
	m_ObjectType |= TYPEMASK_GAMEOBJECT;
	m_ObjectTypeId = TYPEID_GAMEOBJECT;
	m_valuesCount = GAMEOBJECT_END;
}

WoWGameObject::~WoWGameObject()
{
}



//
// Protected
//
std::shared_ptr<WoWGameObject> WoWGameObject::Create(IScene& Scene, ObjectGuid Guid)
{
	std::shared_ptr<WoWGameObject> thisObj = Scene.GetRootSceneNode()->CreateSceneNode<WoWGameObject>(Guid);

	// For test only
	//BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	//bbox.calculateCenter();
	//thisObj->GetComponentT<IColliderComponent>()->SetBounds(bbox);

	return thisObj;
}

void WoWGameObject::AfterCreate(IScene& Scene)
{
	if (m_HiddenNode != nullptr)
	{
		Log::Error("WoWGameObject: Try to call 'AfterCreate' for object.");
		return;
	}

	uint32 displayInfo = GetUInt32Value(GAMEOBJECT_DISPLAYID);
	if (displayInfo != 0)
	{
		CWorldObjectCreator creator(Scene.GetBaseManager());
		m_HiddenNode = creator.BuildGameObjectFromDisplayInfo(Scene.GetBaseManager().GetApplication().GetRenderDevice(), &Scene, displayInfo, shared_from_this());
	}
}

void WoWGameObject::Destroy()
{
	if (m_HiddenNode)
		m_HiddenNode->MakeMeOrphan();
}

#endif
