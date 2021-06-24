#include "stdafx.h"

// General
#include "WorldObjects.h"

// Additional
#include "World.h"
#include "WoWGameObjectMOTransport.h"

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

	for (const auto& wowgameObject : m_WoWGameObject)
		wowgameObject.second->Update(e);

	for (const auto& wowCorpse : m_WoWCorpses)
		wowCorpse.second->Update(e);
}

std::shared_ptr<WoWObject> CWorldObjects::GetWoWObject(CWoWObjectGuid ObjectGUID)
{
	if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_OBJECT)
	{
		const auto& objIterator = m_WoWObjects.find(ObjectGUID);
		if (objIterator != m_WoWObjects.end())
			return objIterator->second;

		auto object = WoWObject::Create(m_Scene, ObjectGUID);
		m_WoWObjects[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_ITEM)
	{
		const auto& objIterator = m_WoWItems.find(ObjectGUID);
		if (objIterator != m_WoWItems.end())
			return objIterator->second;

		auto object = WoWItem::Create(m_Scene, ObjectGUID);
		m_WoWItems[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_CONTAINER)
	{
		const auto& objIterator = m_WoWContainers.find(ObjectGUID);
		if (objIterator != m_WoWContainers.end())
			return objIterator->second;

		auto object = CWoWContainer::Create(m_Scene, ObjectGUID);
		m_WoWContainers[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_UNIT)
	{
		const auto& objIterator = m_WoWUnits.find(ObjectGUID);
		if (objIterator != m_WoWUnits.end())
			return objIterator->second;

		auto object = WoWUnit::Create(m_Scene, ObjectGUID);
		m_WoWUnits[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_PLAYER)
	{
		const auto& objIterator = m_WoWPlayers.find(ObjectGUID);
		if (objIterator != m_WoWPlayers.end())
			return objIterator->second;

		auto object = WoWPlayer::Create(m_Scene, ObjectGUID);
		m_WoWPlayers[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_GAMEOBJECT)
	{
		const auto& objIterator = m_WoWGameObject.find(ObjectGUID);
		if (objIterator != m_WoWGameObject.end())
			return objIterator->second;

		std::shared_ptr<WoWGameObject> object = nullptr;
		if (ObjectGUID.GetHigh() == EWoWObjectHighGuid::Mo_Transport)
			object = WoWGameObjectMOTransport::Create(m_WoWWorld, m_Scene, ObjectGUID);
		else
			object = WoWGameObject::Create(m_WoWWorld, m_Scene, ObjectGUID);
		m_WoWGameObject[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_DYNAMICOBJECT)
	{
		const auto& objIterator = m_WoWDynamicObjects.find(ObjectGUID);
		if (objIterator != m_WoWDynamicObjects.end())
			return objIterator->second;

		auto object = WoWDynamicObject::Create(m_Scene, ObjectGUID);
		m_WoWDynamicObjects[ObjectGUID] = object;
		return object;
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_CORPSE)
	{
		const auto& objIterator = m_WoWCorpses.find(ObjectGUID);
		if (objIterator != m_WoWCorpses.end())
			return objIterator->second;

		auto object = WoWCorpse::Create(m_Scene, ObjectGUID);
		m_WoWCorpses[ObjectGUID] = object;
		return object;
	}

	throw CException("CWorldObjects::IsWoWObjectExists: TypeID '%d' is unknown.", ObjectGUID.GetTypeId());
}

bool CWorldObjects::IsWoWObjectExists(CWoWObjectGuid ObjectGUID)
{
	if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_OBJECT)
	{
		return m_WoWObjects.find(ObjectGUID) != m_WoWObjects.end();
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_ITEM)
	{
		return m_WoWItems.find(ObjectGUID) != m_WoWItems.end();
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_CONTAINER)
	{
		return m_WoWContainers.find(ObjectGUID) != m_WoWContainers.end();
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_UNIT)
	{
		return m_WoWUnits.find(ObjectGUID) != m_WoWUnits.end();
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_PLAYER)
	{
		return m_WoWPlayers.find(ObjectGUID) != m_WoWPlayers.end();
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_GAMEOBJECT)
	{
		return m_WoWGameObject.find(ObjectGUID) != m_WoWGameObject.end();
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_DYNAMICOBJECT)
	{
		return m_WoWDynamicObjects.find(ObjectGUID) != m_WoWDynamicObjects.end();
	}
	else if (ObjectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_CORPSE)
	{
		return m_WoWCorpses.find(ObjectGUID) != m_WoWCorpses.end();
	}

	throw CException("CWorldObjects::IsWoWObjectExists: TypeID '%d' is unknown.", ObjectGUID.GetTypeId());
}

void CWorldObjects::EraseWoWObject(const std::shared_ptr<WoWObject>& WoWObject)
{
	CWoWObjectGuid objectGUID(WoWObject->GetWoWGUID());

	if (objectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_OBJECT)
	{
		auto it = m_WoWObjects.find(objectGUID);
		if (it != m_WoWObjects.end())
			m_WoWObjects.erase(it);
	}
	else if (objectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_ITEM)
	{
		auto it = m_WoWItems.find(objectGUID);
		if (it != m_WoWItems.end())
			m_WoWItems.erase(it);
	}
	else if (objectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_CONTAINER)
	{
		auto it = m_WoWContainers.find(objectGUID);
		if (it != m_WoWContainers.end())
			m_WoWContainers.erase(it);
	}
	else if (objectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_UNIT)
	{
		auto it = m_WoWUnits.find(objectGUID);
		if (it != m_WoWUnits.end())
			m_WoWUnits.erase(it);
	}
	else if (objectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_PLAYER)
	{
		auto it = m_WoWPlayers.find(objectGUID);
		if (it != m_WoWPlayers.end())
			m_WoWPlayers.erase(it);
	}
	else if (objectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_GAMEOBJECT)
	{
		auto it = m_WoWGameObject.find(objectGUID);
		if (it != m_WoWGameObject.end())
			m_WoWGameObject.erase(it);
	}
	else if (objectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_DYNAMICOBJECT)
	{
		auto it = m_WoWDynamicObjects.find(objectGUID);
		if (it != m_WoWDynamicObjects.end())
			m_WoWDynamicObjects.erase(it);
	}
	else if (objectGUID.GetTypeId() == EWoWObjectTypeID::TYPEID_CORPSE)
	{
		auto it = m_WoWCorpses.find(objectGUID);
		if (it != m_WoWCorpses.end())
			m_WoWCorpses.erase(it);
	}
}
