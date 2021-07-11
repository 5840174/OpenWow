#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWGameObject.h"

// Additional
#include "../../World/World.h"

WoWGameObject::WoWGameObject(IScene& Scene, CWoWWorld& WoWWorld, CWoWGuid Guid)
	: CWoWWorldObject(Scene, WoWWorld, Guid)
{
	//m_ObjectType |= TYPEMASK_GAMEOBJECT;
	m_Values.SetValuesCount(GAMEOBJECT_END);
}

WoWGameObject::~WoWGameObject()
{
}

void WoWGameObject::OnValueUpdated(uint16 index)
{
	if (index == OBJECT_FIELD_ENTRY)
	{
		CWoWGuid::EntryType_t objectEntryID = m_Values.GetUInt32Value(index);
		if (m_GameObjectTemplate == nullptr)
		{
			GetWoWWorld().GetClientCache().SendGameObjectQueryResponce(objectEntryID, GetWoWGUID(), std::dynamic_pointer_cast<WoWGameObject>(shared_from_this()));
		}
	}
}

void WoWGameObject::OnValuesUpdated(const UpdateMask & Mask)
{
	if (Mask.GetBit(GAMEOBJECT_DISPLAYID))
	{
		OnDisplayIDChanged(m_Values.GetUInt32Value(GAMEOBJECT_DISPLAYID));
	}
}



//
// IClientCacheGameobjectResponseListener
//
void WoWGameObject::OnTemplate(CWoWGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult)
{
	m_GameObjectTemplate = QueryResult;
}



//
// ISceneNode
//
void WoWGameObject::Update(const UpdateEventArgs & e)
{
	__super::Update(e);
}




//
// Protected
//
std::shared_ptr<WoWGameObject> WoWGameObject::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWGuid Guid)
{
	std::shared_ptr<WoWGameObject> thisObj = MakeShared(WoWGameObject, Scene, WoWWorld, Guid);
	return thisObj;
}

void WoWGameObject::Destroy()
{
	if (m_GameObjectModel)
		m_GameObjectModel->MakeMeOrphan();
}



//
// Protected
//
void WoWGameObject::OnDisplayIDChanged(uint32 DisplayID)
{
	if (m_GameObjectModel != nullptr)
		return;

	//try
	//{
		CWorldObjectCreator creator(GetScene().GetBaseManager());
		m_GameObjectModel = creator.BuildGameObjectFromDisplayInfo(GetScene().GetBaseManager().GetApplication().GetRenderDevice(), GetScene(), DisplayID);

		//const DBC_CreatureDisplayInfoRecord * creatureDisplayInfo = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureDisplayInfo()[diplayID];
		//if (creatureDisplayInfo == nullptr)
		//	throw CException("Creature display info '%d' don't found.", diplayID);

		//const DBC_CreatureModelDataRecord* creatureModelDataRecord = GetBaseManager().GetManager<CDBCStorage>()->DBC_CreatureModelData()[creatureDisplayInfo->Get_Model()];
		//if (creatureModelDataRecord == nullptr)
		//	throw CException("Creature model data '%d' don't found.", creatureDisplayInfo->Get_Model());

		//float scaleFromCreature = creatureDisplayInfo->Get_Scale();
		//float scaleFromModel = creatureModelDataRecord->Get_Scale();
		float scale = 1.0f;
		if (m_Values.IsExists(OBJECT_FIELD_SCALE_X))
			scale = m_Values.GetFloatValue(OBJECT_FIELD_SCALE_X);
		m_GameObjectModel->SetScale(glm::vec3(scale));


	//}
	//catch (const CException& e)
	//{
	//	Log::Error("WoWGameObject::AfterCreate: Exception '%s'.", e.MessageCStr());
	//}
}

#endif
