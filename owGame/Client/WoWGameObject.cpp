#include "stdafx.h"

// General
#include "WoWGameObject.h"

// Additional
#include "World/WorldObjectsCreator.h"
#include "..\World\GameObject\GameObject.h"

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
	thisObj->m_valuesCount = GAMEOBJECT_END;

	// For test only
	BoundingBox bbox(glm::vec3(-2.0f), glm::vec3(2.0f));
	bbox.calculateCenter();
	//thisObj->GetComponent<IColliderComponent3D>()->SetBounds(bbox);

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

