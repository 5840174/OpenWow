#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWGameObject.h"

// Additional
#include "../../World/ServerWorld.h"

CowServerGameObject::CowServerGameObject(IScene& Scene, CowServerWorld& WoWWorld, CowGuid Guid)
	: CowServerWorldObject(Scene, WoWWorld, Guid)
{
	//m_ObjectType |= TYPEMASK_GAMEOBJECT;
	m_Values.SetValuesCount(GAMEOBJECT_END);
}

CowServerGameObject::~CowServerGameObject()
{
}

void CowServerGameObject::OnValueUpdated(uint16 index)
{
	if (index == OBJECT_FIELD_ENTRY)
	{
		CowGuid::EntryType_t objectEntryID = m_Values.GetUInt32Value(index);
		if (m_GameObjectTemplate == nullptr)
		{
			GetWoWWorld().GetClientCache().SendGameObjectQueryResponce(objectEntryID, GetWoWGUID(), std::dynamic_pointer_cast<CowServerGameObject>(shared_from_this()));
		}
	}
}

void CowServerGameObject::OnValuesUpdated(const UpdateMask & Mask)
{
	if (Mask.GetBit(GAMEOBJECT_DISPLAYID))
	{
		OnDisplayIDChanged(m_Values.GetUInt32Value(GAMEOBJECT_DISPLAYID));
	}
}



//
// IClientCacheGameobjectResponseListener
//
void CowServerGameObject::OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult)
{
	m_GameObjectTemplate = QueryResult;
}



//
// ISceneNode
//
void CowServerGameObject::Update(const UpdateEventArgs & e)
{
	__super::Update(e);
}




//
// Protected
//
std::shared_ptr<CowServerGameObject> CowServerGameObject::Create(CowServerWorld& WoWWorld, IScene& Scene, CowGuid Guid)
{
	std::shared_ptr<CowServerGameObject> thisObj = MakeShared(CowServerGameObject, Scene, WoWWorld, Guid);
	return thisObj;
}

void CowServerGameObject::Destroy()
{
	if (m_GameObjectModel)
		m_GameObjectModel->MakeMeOrphan();
}



//
// Protected
//
void CowServerGameObject::OnDisplayIDChanged(uint32 DisplayID)
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
	//	Log::Error("CowServerGameObject::AfterCreate: Exception '%s'.", e.MessageCStr());
	//}
}

#endif
