#include "stdafx.h"

#if 0

// General
#include "WoWGameObject.h"

// Additional
#include "World/WorldObjectsCreator.h"
#include "..\World\GameObject\GameObject.h"

WoWGameObject::WoWGameObject(ObjectGuid Guid)
	: WorldObject(Guid)
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
std::shared_ptr<WoWGameObject> WoWGameObject::Create(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene, ObjectGuid Guid)
{
	std::shared_ptr<WoWGameObject> thisObj = Scene->GetRootNode3D()->CreateSceneNode<WoWGameObject>(Guid);
	Log::Green("WoWGameObject created!");

	// For test only
	BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	bbox.calculateCenter();
	//thisObj->GetComponentT<IColliderComponent>()->SetBounds(bbox);

	return thisObj;
}

void WoWGameObject::AfterCreate(IBaseManager & BaseManager, IRenderDevice & RenderDevice, IScene * Scene)
{
	uint32 displayInfo = GetUInt32Value(GAMEOBJECT_DISPLAYID);
	if (displayInfo != 0)
	{
		CWorldObjectCreator creator(BaseManager);
		m_HiddenNode = creator.BuildGameObjectFromDisplayInfo(RenderDevice, Scene, displayInfo, shared_from_this());
	}
}

#endif
