#include "stdafx.h"

// General
#include "WorldObjects.h"

// Additional
#include "ServerWorld.h"
#include "../Objects/GameObjects/WoWGameObjectMOTransport.h"

CWorldObjects::CWorldObjects(CowServerWorld & WoWWorld, IScene & Scene)
	: m_ServerWorld(WoWWorld)
	, m_Scene(Scene)
{}

CWorldObjects::~CWorldObjects()
{}

void CWorldObjects::Update(const UpdateEventArgs & e)
{
	for (const auto& wowUnit : m_Units)
		wowUnit.second->Update(e);

	for (const auto& wowPlayer : m_Players)
		wowPlayer.second->Update(e);

	for (const auto& wowGameObject : m_GameObjects)
		wowGameObject.second->Update(e);

	//for (const auto& wowCorpse : m_Corpses)
	//	wowCorpse.second->Update(e);

	for (const auto& wowCorpse : m_Corpses)
		wowCorpse.second->Update(e);
}

void CWorldObjects::Accept(IWoWVisitor * WoWVisitor)
{
	for (const auto& wowUnit : m_Units)
		WoWVisitor->VisitWoW(wowUnit.second);

	for (const auto& wowGameObject : m_GameObjects)
		WoWVisitor->VisitWoW(wowGameObject.second);
}

std::shared_ptr<CowServerObject> CWorldObjects::Create(CowGuid ObjectGUID, EWoWObjectTypeID TypeID)
{
	if (auto wowObject = Get(ObjectGUID))
		return wowObject;

	// Item or container
	if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Item)
	{
		if (TypeID == EWoWObjectTypeID::TYPEID_ITEM)
		{
			auto object = CowServerItem::Create(m_Scene, ObjectGUID);
			m_ItemsAndContainers[ObjectGUID] = object;
			return object;
		}
		else if (TypeID == EWoWObjectTypeID::TYPEID_CONTAINER)
		{
			auto object = CowServerContainer::Create(m_Scene, ObjectGUID);
			m_ItemsAndContainers[ObjectGUID] = object;
			return object;
		}
		else
			throw CException("CWorldObjects::Create: For 'Item' TypeID must be TYPEID_ITEM or TYPEID_CONTAINER. TypeID '%d'.", TypeID);
	}


	// Unit
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Unit)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_UNIT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerUnit::Create(m_ServerWorld, m_Scene, ObjectGUID);
		m_Units[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Pet)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_UNIT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerUnit::Create(m_ServerWorld, m_Scene, ObjectGUID); // TODO: To Pet
		m_Units_Pet[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Vehicle)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_UNIT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerUnit::Create(m_ServerWorld, m_Scene, ObjectGUID); // TODO: To Vehicle
		m_Units_Vehicle[ObjectGUID] = object;
		return object;
	}


	// Player
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Player)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_PLAYER)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerPlayer::Create(m_ServerWorld, m_Scene, ObjectGUID);
		m_Players[ObjectGUID] = object;
		return object;
	}


	// GameObjects
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::GameObject)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_GAMEOBJECT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerGameObject::Create(m_ServerWorld, m_Scene, ObjectGUID);
		m_GameObjects[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Transport)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_GAMEOBJECT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerGameObject::Create(m_ServerWorld, m_Scene, ObjectGUID);
		m_GameObjects_Transport[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_GAMEOBJECT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerGameObject_MOTransport::Create(m_ServerWorld, m_Scene, ObjectGUID);
		m_GameObjects_MOTransport[ObjectGUID] = object;
		return object;
	}


	// DynamicObject
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::DynamicObject)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_DYNAMICOBJECT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerDynamicObject::Create(m_Scene, ObjectGUID);
		m_DynamicObjects[ObjectGUID] = object;
		return object;
	}


	// Corpse
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Corpse)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_CORPSE)
			throw CException("HighGuid mismatch TypeID.");

		auto object = CowServerCorpse::Create(m_ServerWorld, m_Scene, ObjectGUID);
		m_Corpses[ObjectGUID] = object;
		return object;
	}

	throw CException("CWorldObjects::Create: ObjectGUID.GetHigh() '%d' is unknown.", ObjectGUID.GetHigh());
}

std::shared_ptr<CowServerObject> CWorldObjects::Get(CowGuid ObjectGUID)
{
	// Items & Containers
	if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Item || ObjectGUID.GetHigh() == EWoWObjectHighGuid::Container)
	{
		const auto& objIterator = m_ItemsAndContainers.find(ObjectGUID);
		if (objIterator != m_ItemsAndContainers.end())
			return objIterator->second;
		return nullptr;
	}


	// Unit
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Unit)
	{
		const auto& objIterator = m_Units.find(ObjectGUID);
		if (objIterator != m_Units.end())
			return objIterator->second;
		return nullptr;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Pet)
	{
		const auto& objIterator = m_Units_Pet.find(ObjectGUID);
		if (objIterator != m_Units_Pet.end())
			return objIterator->second;
		return nullptr;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Vehicle)
	{
		const auto& objIterator = m_Units_Vehicle.find(ObjectGUID);
		if (objIterator != m_Units_Vehicle.end())
			return objIterator->second;
		return nullptr;
	}


	// Player
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Player)
	{
		const auto& objIterator = m_Players.find(ObjectGUID);
		if (objIterator != m_Players.end())
			return objIterator->second;
		return nullptr;
	}


	// GameObjects
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::GameObject)
	{
		const auto& objIterator = m_GameObjects.find(ObjectGUID);
		if (objIterator != m_GameObjects.end())
			return objIterator->second;
		return nullptr;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Transport)
	{
		const auto& objIterator = m_GameObjects_Transport.find(ObjectGUID);
		if (objIterator != m_GameObjects_Transport.end())
			return objIterator->second;
		return nullptr;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		const auto& objIterator = m_GameObjects_MOTransport.find(ObjectGUID);
		if (objIterator != m_GameObjects_MOTransport.end())
			return objIterator->second;
		return nullptr;
	}


	// DynamicObject
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::DynamicObject)
	{
		const auto& objIterator = m_DynamicObjects.find(ObjectGUID);
		if (objIterator != m_DynamicObjects.end())
			return objIterator->second;
		return nullptr;
	}


	// Corpse
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Corpse)
	{
		const auto& objIterator = m_Corpses.find(ObjectGUID);
		if (objIterator != m_Corpses.end())
			return objIterator->second;
		return nullptr;
	}

	throw CException("CWorldObjects::Get: ObjectGUID.GetHigh() '%d' is unknown.", ObjectGUID.GetHigh());
}

bool CWorldObjects::IsExists(CowGuid ObjectGUID)
{
	// Items & Containers
	if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Item || ObjectGUID.GetHigh() == EWoWObjectHighGuid::Container)
	{
		return m_ItemsAndContainers.find(ObjectGUID) != m_ItemsAndContainers.end();
	}

	// Unit
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Unit)
	{
		return m_Units.find(ObjectGUID) != m_Units.end();
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Pet)
	{
		return m_Units_Pet.find(ObjectGUID) != m_Units_Pet.end();
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Vehicle)
	{
		return m_Units_Vehicle.find(ObjectGUID) != m_Units_Vehicle.end();
	}

	// Player
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Player)
	{
		return m_Players.find(ObjectGUID) != m_Players.end();
	}

	// GameObjects
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::GameObject)
	{
		return m_GameObjects.find(ObjectGUID) != m_GameObjects.end();
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Transport)
	{
		return m_GameObjects_Transport.find(ObjectGUID) != m_GameObjects_Transport.end();
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		return m_GameObjects_MOTransport.find(ObjectGUID) != m_GameObjects_MOTransport.end();
	}

	// DynamicObject
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::DynamicObject)
	{
		return m_DynamicObjects.find(ObjectGUID) != m_DynamicObjects.end();
	}

	// Corpse
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Corpse)
	{
		return m_Corpses.find(ObjectGUID) != m_Corpses.end();
	}

	throw CException("CWorldObjects::IsExists: TypeID '%d' is unknown.", ObjectGUID.GetHigh());
}

void CWorldObjects::Delete(const std::shared_ptr<CowServerObject>& WoWObject)
{
	CowGuid objectGUID(WoWObject->GetWoWGUID());

	// Items & Containers
	if (objectGUID.GetHigh() == EWoWObjectHighGuid::Item || objectGUID.GetHigh() == EWoWObjectHighGuid::Container)
	{
		const auto& it = m_ItemsAndContainers.find(objectGUID);
		if (it != m_ItemsAndContainers.end())
			m_ItemsAndContainers.erase(it);
	}

	// Unit
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Unit)
	{
		const auto& it = m_Units.find(objectGUID);
		if (it != m_Units.end())
			m_Units.erase(it);
	}
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Pet)
	{
		const auto& it = m_Units_Pet.find(objectGUID);
		if (it != m_Units_Pet.end())
			m_Units_Pet.erase(it);
	}
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Vehicle)
	{
		const auto& it = m_Units_Vehicle.find(objectGUID);
		if (it != m_Units_Vehicle.end())
			m_Units_Vehicle.erase(it);
	}

	// Player
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Player)
	{
		const auto& it = m_Players.find(objectGUID);
		if (it != m_Players.end())
			m_Players.erase(it);
	}

	// GameObject
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::GameObject)
	{
		const auto& it = m_GameObjects.find(objectGUID);
		if (it != m_GameObjects.end())
			m_GameObjects.erase(it);
	}
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Transport)
	{
		const auto& it = m_GameObjects_Transport.find(objectGUID);
		if (it != m_GameObjects_Transport.end())
			m_GameObjects_Transport.erase(it);
	}
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		const auto& it = m_GameObjects_MOTransport.find(objectGUID);
		if (it != m_GameObjects_MOTransport.end())
			m_GameObjects_MOTransport.erase(it);
	}

	// DynamicObject
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::DynamicObject)
	{
		const auto& it = m_DynamicObjects.find(objectGUID);
		if (it != m_DynamicObjects.end())
			m_DynamicObjects.erase(it);
	}

	// Corpse
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Corpse)
	{
		const auto& it = m_Corpses.find(objectGUID);
		if (it != m_Corpses.end())
			m_Corpses.erase(it);
	}

	//throw CException("CWorldObjects::Delete: TypeID '%d' is unknown.", objectGUID.GetHigh());
}
