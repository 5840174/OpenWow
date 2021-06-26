#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWGameObject.h"

// Additional
#include "../World.h"

WoWGameObject::WoWGameObject(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid)
	: CWoWWorldObject(Scene, WoWWorld, Guid)
	, templateCreated(false)
{
	m_ObjectType |= TYPEMASK_GAMEOBJECT;
	m_Values.SetValuesCount(GAMEOBJECT_END);
}

WoWGameObject::~WoWGameObject()
{
}

void WoWGameObject::OnValueUpdated(uint16 index)
{
	if (index == OBJECT_FIELD_ENTRY)
	{
		CWoWObjectGuid::EntryType_t obtainedEntry = m_Values.GetUInt32Value(index);
		if (obtainedEntry != 0)
		{
			if (false == templateCreated && m_GameObjectTemplate == nullptr)
			{
				GetWoWWorld().GetClientCache().SendQueryResponceWithCallback(obtainedEntry, GetWoWGUID(), std::bind(&WoWGameObject::OnTemplateCallback, this, std::placeholders::_1, std::placeholders::_2));
				templateCreated = true;
			}
		}
	}
}

void WoWGameObject::OnValuesUpdated(const UpdateMask & Mask)
{
	if (Mask.GetBit(GAMEOBJECT_DISPLAYID))
	{
		if (m_HiddenNode != nullptr)
		{
			//Log::Warn("WoWUnit: UNIT_FIELD_DISPLAYID updated, but Node already exists.");
			return;
		}

		try
		{
			CWorldObjectCreator creator(GetScene().GetBaseManager());
			m_HiddenNode = creator.BuildGameObjectFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), m_Values.GetUInt32Value(GAMEOBJECT_DISPLAYID));
		}
		catch (const CException& e)
		{
			Log::Error("WoWGameObject::AfterCreate: Exception '%s'.", e.MessageCStr());
		}
	}
}



//
// ISceneNode
//
void WoWGameObject::Update(const UpdateEventArgs & e)
{
	__super::Update(e);

}

void WoWGameObject::OnTemplateCallback(CWoWObjectGuid::EntryType_t entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult)
{
	//uint32 idFromValues = GetUInt32Value(OBJECT_FIELD_ENTRY);
	//if (idFromValues != entry)
	//{
	//	Log::Error("WoWGameObject::OnTemplateCallback: %d to %d.", idFromValues, entry);
	//	return;
	//}

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

}

void WoWGameObject::Destroy()
{
	if (m_HiddenNode)
		m_HiddenNode->MakeMeOrphan();
}

#endif
