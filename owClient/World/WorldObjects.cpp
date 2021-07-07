#include "stdafx.h"

// General
#include "WorldObjects.h"

// Additional
#include "World.h"
#include "Objects/WoWGameObjectMOTransport.h"

CWorldObjects::CWorldObjects(CWoWWorld & WoWWorld, IScene & Scene)
	: m_WoWWorld(WoWWorld)
	, m_Scene(Scene)
{}

CWorldObjects::~CWorldObjects()
{}

void CWorldObjects::Update(const UpdateEventArgs & e)
{
	for (const auto& wowUnit : m_WoWUnits)
		wowUnit.second->Update(e);

	for (const auto& wowPlayer : m_WoWPlayers)
		wowPlayer.second->Update(e);

	for (const auto& wowGameObject : m_WoWGameObjects)
		wowGameObject.second->Update(e);

	for (const auto& wowCorpse : m_WoWCorpses)
		wowCorpse.second->Update(e);
}

void CWorldObjects::Accept(IWoWVisitor * WoWVisitor)
{
	for (const auto& wowUnit : m_WoWUnits)
		WoWVisitor->VisitWoW(wowUnit.second);

	for (const auto& wowGameObject : m_WoWGameObjects)
		WoWVisitor->VisitWoW(wowGameObject.second);
}

std::shared_ptr<WoWObject> CWorldObjects::CreateWoWObject(CWoWGuid ObjectGUID, EWoWObjectTypeID TypeID)
{
	if (auto wowObject = GetWoWObject(ObjectGUID))
		return wowObject;

	// Item or container
	if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Item)
	{
		if (TypeID == EWoWObjectTypeID::TYPEID_ITEM)
		{
			auto object = WoWItem::Create(m_Scene, ObjectGUID);
			m_WoWItems[ObjectGUID] = object;
			return object;
		}
		else if (TypeID == EWoWObjectTypeID::TYPEID_CONTAINER)
		{
			auto object = CWoWContainer::Create(m_Scene, ObjectGUID);
			m_WoWItems[ObjectGUID] = object;
			return object;
		}
		else
			throw CException("CWorldObjects::CreateWoWObject: For 'Item' TypeID must be TYPEID_ITEM or TYPEID_CONTAINER. TypeID '%d'.", TypeID);
	}


	// Unit
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Unit)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_UNIT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWUnit::Create(m_WoWWorld, m_Scene, ObjectGUID);
		m_WoWUnits[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Pet)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_UNIT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWUnit::Create(m_WoWWorld, m_Scene, ObjectGUID); // TODO: To Pet
		m_WoWUnitsPet[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Vehicle)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_UNIT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWUnit::Create(m_WoWWorld, m_Scene, ObjectGUID); // TODO: To Vehicle
		m_WoWUnitsVehicle[ObjectGUID] = object;
		return object;
	}


	// Player
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Player)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_PLAYER)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWPlayer::Create(m_WoWWorld, m_Scene, ObjectGUID);
		m_WoWPlayers[ObjectGUID] = object;
		return object;
	}


	// GameObjects
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::GameObject)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_GAMEOBJECT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWGameObject::Create(m_WoWWorld, m_Scene, ObjectGUID);
		m_WoWGameObjects[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Transport)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_GAMEOBJECT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWGameObject::Create(m_WoWWorld, m_Scene, ObjectGUID);
		m_WoWGameObjectsTransport[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_GAMEOBJECT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWGameObjectMOTransport::Create(m_WoWWorld, m_Scene, ObjectGUID);
		m_WoWGameObjectsMOTransport[ObjectGUID] = object;
		return object;
	}


	// DynamicObject
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::DynamicObject)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_DYNAMICOBJECT)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWDynamicObject::Create(m_Scene, ObjectGUID);
		m_WoWDynamicObjects[ObjectGUID] = object;
		return object;
	}


	// Corpse
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Corpse)
	{
		if (TypeID != EWoWObjectTypeID::TYPEID_CORPSE)
			throw CException("HighGuid mismatch TypeID.");

		auto object = WoWCorpse::Create(m_WoWWorld, m_Scene, ObjectGUID);
		m_WoWCorpses[ObjectGUID] = object;
		return object;
	}

	throw CException("CWorldObjects::CreateWoWObject: ObjectGUID.GetHigh() '%d' is unknown.", ObjectGUID.GetHigh());
}

std::shared_ptr<WoWObject> CWorldObjects::GetWoWObject(CWoWGuid ObjectGUID)
{
	// Items & Containers
	if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Item)
	{
		const auto& objIterator = m_WoWItems.find(ObjectGUID);
		if (objIterator != m_WoWItems.end())
			return objIterator->second;
		return nullptr;
	}


	// Unit
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Unit)
	{
		const auto& objIterator = m_WoWUnits.find(ObjectGUID);
		if (objIterator != m_WoWUnits.end())
			return objIterator->second;
		return nullptr;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Pet)
	{
		const auto& objIterator = m_WoWUnitsPet.find(ObjectGUID);
		if (objIterator != m_WoWUnitsPet.end())
			return objIterator->second;
		return nullptr;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Vehicle)
	{
		const auto& objIterator = m_WoWUnitsVehicle.find(ObjectGUID);
		if (objIterator != m_WoWUnitsVehicle.end())
			return objIterator->second;
		return nullptr;
	}


	// Player
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Player)
	{
		const auto& objIterator = m_WoWPlayers.find(ObjectGUID);
		if (objIterator != m_WoWPlayers.end())
			return objIterator->second;
		return nullptr;
	}


	// GameObjects
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::GameObject)
	{
		const auto& objIterator = m_WoWGameObjects.find(ObjectGUID);
		if (objIterator != m_WoWGameObjects.end())
			return objIterator->second;
		return nullptr;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Transport)
	{
		const auto& objIterator = m_WoWGameObjectsTransport.find(ObjectGUID);
		if (objIterator != m_WoWGameObjectsTransport.end())
			return objIterator->second;
		return nullptr;
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		const auto& objIterator = m_WoWGameObjectsMOTransport.find(ObjectGUID);
		if (objIterator != m_WoWGameObjectsMOTransport.end())
			return objIterator->second;
		return nullptr;
	}


	// DynamicObject
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::DynamicObject)
	{
		const auto& objIterator = m_WoWDynamicObjects.find(ObjectGUID);
		if (objIterator != m_WoWDynamicObjects.end())
			return objIterator->second;
		return nullptr;
	}


	// Corpse
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Corpse)
	{
		const auto& objIterator = m_WoWCorpses.find(ObjectGUID);
		if (objIterator != m_WoWCorpses.end())
			return objIterator->second;
		return nullptr;
	}

	throw CException("CWorldObjects::GetWoWObject: ObjectGUID.GetHigh() '%d' is unknown.", ObjectGUID.GetHigh());
}

bool CWorldObjects::IsWoWObjectExists(CWoWGuid ObjectGUID)
{
	// Items & Containers
	if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Item)
	{
		return m_WoWItems.find(ObjectGUID) != m_WoWItems.end();
	}

	// Unit
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Unit)
	{
		return m_WoWUnits.find(ObjectGUID) != m_WoWUnits.end();
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Pet)
	{
		return m_WoWUnitsPet.find(ObjectGUID) != m_WoWUnitsPet.end();
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Vehicle)
	{
		return m_WoWUnitsVehicle.find(ObjectGUID) != m_WoWUnitsVehicle.end();
	}

	// Player
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Player)
	{
		return m_WoWPlayers.find(ObjectGUID) != m_WoWPlayers.end();
	}

	// GameObjects
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::GameObject)
	{
		return m_WoWGameObjects.find(ObjectGUID) != m_WoWGameObjects.end();
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Transport)
	{
		return m_WoWGameObjectsTransport.find(ObjectGUID) != m_WoWGameObjectsTransport.end();
	}
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		return m_WoWGameObjectsMOTransport.find(ObjectGUID) != m_WoWGameObjectsMOTransport.end();
	}

	// DynamicObject
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::DynamicObject)
	{
		return m_WoWDynamicObjects.find(ObjectGUID) != m_WoWDynamicObjects.end();
	}

	// Corpse
	else if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Corpse)
	{
		return m_WoWCorpses.find(ObjectGUID) != m_WoWCorpses.end();
	}

	throw CException("CWorldObjects::IsWoWObjectExists: TypeID '%d' is unknown.", ObjectGUID.GetHigh());
}

void CWorldObjects::EraseWoWObject(const std::shared_ptr<WoWObject>& WoWObject)
{
	CWoWGuid objectGUID(WoWObject->GetWoWGUID());

	// Items & Containers
	if (objectGUID.GetHigh() == EWoWObjectHighGuid::Item)
	{
		const auto& it = m_WoWItems.find(objectGUID);
		if (it != m_WoWItems.end())
			m_WoWItems.erase(it);
	}

	// Unit
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Unit)
	{
		const auto& it = m_WoWUnits.find(objectGUID);
		if (it != m_WoWUnits.end())
			m_WoWUnits.erase(it);
	}
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Pet)
	{
		const auto& it = m_WoWUnitsPet.find(objectGUID);
		if (it != m_WoWUnitsPet.end())
			m_WoWUnitsPet.erase(it);
	}
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Vehicle)
	{
		const auto& it = m_WoWUnitsVehicle.find(objectGUID);
		if (it != m_WoWUnitsVehicle.end())
			m_WoWUnitsVehicle.erase(it);
	}

	// Player
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Player)
	{
		const auto& it = m_WoWPlayers.find(objectGUID);
		if (it != m_WoWPlayers.end())
			m_WoWPlayers.erase(it);
	}

	// GameObject
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::GameObject)
	{
		const auto& it = m_WoWGameObjects.find(objectGUID);
		if (it != m_WoWGameObjects.end())
			m_WoWGameObjects.erase(it);
	}
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Transport)
	{
		const auto& it = m_WoWGameObjectsTransport.find(objectGUID);
		if (it != m_WoWGameObjectsTransport.end())
			m_WoWGameObjectsTransport.erase(it);
	}
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
	{
		const auto& it = m_WoWGameObjectsMOTransport.find(objectGUID);
		if (it != m_WoWGameObjectsMOTransport.end())
			m_WoWGameObjectsMOTransport.erase(it);
	}

	// DynamicObject
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::DynamicObject)
	{
		const auto& it = m_WoWDynamicObjects.find(objectGUID);
		if (it != m_WoWDynamicObjects.end())
			m_WoWDynamicObjects.erase(it);
	}

	// Corpse
	else if (objectGUID.GetHigh() == EWoWObjectHighGuid::Corpse)
	{
		const auto& it = m_WoWCorpses.find(objectGUID);
		if (it != m_WoWCorpses.end())
			m_WoWCorpses.erase(it);
	}

	//throw CException("CWorldObjects::EraseWoWObject: TypeID '%d' is unknown.", objectGUID.GetHigh());
}
