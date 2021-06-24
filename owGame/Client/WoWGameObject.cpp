#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWGameObject.h"

// Additional
#include "World.h"
#include "World/WorldObjectsCreator.h"

WoWGameObject::WoWGameObject(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid)
	: CWoWWorldObject(Scene, Guid)
	, m_WoWWorld(WoWWorld)
	, templateCreated(false)
{
	m_ObjectType |= TYPEMASK_GAMEOBJECT;
	m_ObjectTypeId = TYPEID_GAMEOBJECT;
	m_valuesCount = GAMEOBJECT_END;
}

WoWGameObject::~WoWGameObject()
{
}



//
// ISceneNode
//
void WoWGameObject::Update(const UpdateEventArgs & e)
{
	__super::Update(e);

	CWoWObjectGuid::EntryType_t obtainedEntry = GetUInt32Value(OBJECT_FIELD_ENTRY);
	if (obtainedEntry != 0)
	{
		if (false == templateCreated && m_GameObjectTemplate == nullptr)
		{
			m_WoWWorld.GetClientCache().SendQueryResponceWithCallback(obtainedEntry, GetWoWGUID(), std::bind(&WoWGameObject::OnTemplateCallback, this, std::placeholders::_1, std::placeholders::_2));
			templateCreated = true;

			return;
		}
	}
}

void WoWGameObject::OnTemplateCallback(CWoWObjectGuid::EntryType_t entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult)
{
	if (GetUInt32Value(OBJECT_FIELD_ENTRY) != entry)
		throw CException("Template Entry type mismatch.");

	m_GameObjectTemplate = QueryResult;
}



//
// Protected
//
std::shared_ptr<WoWGameObject> WoWGameObject::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWGameObject> thisObj = MakeShared(WoWGameObject, Scene, WoWWorld, Guid);
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
		m_HiddenNode = creator.BuildGameObjectFromDisplayInfo(Scene.GetBaseManager().GetApplication().GetRenderDevice(), &Scene, displayInfo);
	}
	else
		throw CException("GameObject display info is zero.");
}

void WoWGameObject::Destroy()
{
	if (m_HiddenNode)
		m_HiddenNode->MakeMeOrphan();
}

#endif
