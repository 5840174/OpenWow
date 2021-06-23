#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "WoWGameObject.h"

// Additional
#include "World.h"
#include "World/WorldObjectsCreator.h"

#include "TaxiStorage.h"

WoWGameObject::WoWGameObject(IScene& Scene, CWoWWorld& WoWWorld, CWoWObjectGuid Guid)
	: WorldObject(Scene, Guid)
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

void WoWGameObject::Update(const UpdateEventArgs & e)
{
	CWoWObjectGuid::EntryType_t obtainedEntry = GetUInt32Value(OBJECT_FIELD_ENTRY);
	if (obtainedEntry == 0)
	{
		return;
	}

	if (false == templateCreated && m_GameObjectTemplate == nullptr)
	{
		m_WoWWorld.GetClientCache().SendQueryResponceWithCallback(obtainedEntry, GetWoWGUID(), std::bind(&WoWGameObject::OnTemplateCallback, this, std::placeholders::_1, std::placeholders::_2));
		templateCreated = true;

		return;
	}

	

	if (/*GetWoWGUID().GetHigh() == EWoWObjectHighGuid::Transport ||*/ GetWoWGUID().GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		GameobjectTypes type = GameobjectTypes(GetByteValue(GAMEOBJECT_BYTES_1, 1));
		if (type != GameobjectTypes::GAMEOBJECT_TYPE_TRANSPORT && type != GameobjectTypes::GAMEOBJECT_TYPE_MO_TRANSPORT)
		{
			throw CException("FUCK");
		}

		int16 pathProgress = GetInt16Value(GAMEOBJECT_DYNAMIC, 1);
		//Log::Warn("Path progress: %d", pathProgress);
		//if (pathProgress == -1)
		//	return;

		size_t a = m_WoWWorld.GetTransportAnimationStorage().GetTransportAnimations(GetUInt32Value(OBJECT_FIELD_ENTRY)).size();

		float pathProgressFloat = float(pathProgress) / 65535.0f;

		//Log::Print("Hello world! %d (%d) - (int %d) (float %f)", GetUInt32Value(OBJECT_FIELD_ENTRY), a, pathProgress, pathProgressFloat);
	}
}

void WoWGameObject::OnTemplateCallback(CWoWObjectGuid::EntryType_t entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult)
{
	if (GetUInt32Value(OBJECT_FIELD_ENTRY) != entry)
		throw CException("FUCK!");

	if (GetWoWGUID().GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		Log::Print("OnTemplateCallback: EWoWObjectHighGuid::Mo_Transport.");


		uint32 taxiPathID = QueryResult->moTransport.taxiPathId;
		size_t size = m_WoWWorld.GetTaxiStorage().GetPathNodes(taxiPathID).size();
		Log::Warn("Pathssssss size '%d'.", size);
	}
}

void WoWGameObject::ProcessMovementUpdate(CByteBuffer & Bytes)
{
	if (GetWoWGUID().GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		//printf("Hello World!");
	}

	__super::ProcessMovementUpdate(Bytes);
}



//
// Protected
//
std::shared_ptr<WoWGameObject> WoWGameObject::Create(CWoWWorld& WoWWorld, IScene& Scene, CWoWObjectGuid Guid)
{
	std::shared_ptr<WoWGameObject> thisObj = Scene.GetRootSceneNode()->CreateSceneNode<WoWGameObject>(WoWWorld, Guid);
	Log::Green("---Create object with Type '%s', Entry '%d', Counter '%d'", Guid.GetTypeName(), Guid.GetEntry(), Guid.GetCounter());

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
