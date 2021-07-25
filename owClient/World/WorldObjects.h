#pragma once

#include "../Objects/Base/WoWObject.h"
#include "../Objects/Items/WoWItem.h"
#include "../Objects/Items/WoWContainer.h"
#include "../Objects/Units/WoWUnit.h"
#include "../Objects/GameObjects/WoWGameObject.h"

#include "../Objects/WoWPlayer.h"
#include "../Objects/WoWDynamicObject.h"
#include "../Objects/WoWCorpse.h"

#include "../Renderer/RenderPass_Path.h"

// FORWARD BEGIN
class CWorldServer;
// FORWARD END

class CWorldObjects
{
public:
	CWorldObjects(CWorldServer& WoWWorld, IScene& Scene);
	virtual ~CWorldObjects();

	void Update(const UpdateEventArgs& e);
	void Accept(IWoWVisitor * WoWVisitor);

	std::shared_ptr<CowServerObject> Create(CowGuid ObjectGUID, EWoWObjectTypeID TypeID);
	std::shared_ptr<CowServerObject> Get(CowGuid ObjectGUID);
	bool                       IsExists(CowGuid ObjectGUID);
	void                       Delete(const std::shared_ptr<CowServerObject>& WoWObject);

	template <typename T>
	std::shared_ptr<T> GetT(CowGuid ObjectGUID)
	{
		if (auto object = Get(ObjectGUID))
		{
			if (auto objectAsT = std::dynamic_pointer_cast<T>(object))
			{
				return objectAsT;
			}
			else
				throw CException("Unknown cast.");
		}
		return nullptr; // Not found
	}

private:
	CWorldServer& m_ServerWorld;
	IScene& m_Scene;

	std::map<CowGuid, std::shared_ptr<CowServerItem>>                  m_ItemsAndContainers;

	std::map<CowGuid, std::shared_ptr<CowServerUnit>>                  m_Units;
	std::map<CowGuid, std::shared_ptr<CowServerUnit>>                  m_Units_Pet;
	std::map<CowGuid, std::shared_ptr<CowServerUnit>>                  m_Units_Vehicle;

	std::map<CowGuid, std::shared_ptr<CowServerPlayer>>                m_Players;

	std::map<CowGuid, std::shared_ptr<CowServerGameObject>>            m_GameObjects;
	std::map<CowGuid, std::shared_ptr<CowServerGameObject>>            m_GameObjects_Transport;
	std::map<CowGuid, std::shared_ptr<CowServerGameObject_MOTransport>> m_GameObjects_MOTransport;

	std::map<CowGuid, std::shared_ptr<CowServerDynamicObject>>         m_DynamicObjects;

	std::map<CowGuid, std::shared_ptr<CowServerCorpse>>                m_Corpses;
};