#pragma once

#include "Objects/WoWObject.h"
#include "Objects/WoWItem.h"
#include "Objects/WoWContainer.h"
#include "Objects/WoWUnit.h"
#include "Objects/WoWPlayer.h"
#include "Objects/WoWGameObject.h"
#include "Objects/WoWDynamicObject.h"
#include "Objects/WoWCorpse.h"

#include "../Renderer/RenderPass_Path.h"

// FORWARD BEGIN
class CWoWWorld;
// FORWARD END

class CWorldObjects
{
public:
	CWorldObjects(CWoWWorld& WoWWorld, IScene& Scene);
	virtual ~CWorldObjects();

	void Update(const UpdateEventArgs& e);
	void Accept(IWoWVisitor * WoWVisitor);

	std::shared_ptr<WoWObject> GetWoWObject(CWoWObjectGuid ObjectGUID);
	bool IsWoWObjectExists(CWoWObjectGuid ObjectGUID);
	void EraseWoWObject(const std::shared_ptr<WoWObject>& WoWObject);



private:
	CWoWWorld& m_WoWWorld;
	IScene& m_Scene;
	std::map<CWoWObjectGuid, std::shared_ptr<WoWObject>>         m_WoWObjects;
	std::map<CWoWObjectGuid, std::shared_ptr<WoWItem>>           m_WoWItems;
	std::map<CWoWObjectGuid, std::shared_ptr<CWoWContainer>>     m_WoWContainers;
	std::map<CWoWObjectGuid, std::shared_ptr<WoWUnit>>           m_WoWUnits;
	std::map<CWoWObjectGuid, std::shared_ptr<WoWPlayer>>         m_WoWPlayers;
	std::map<CWoWObjectGuid, std::shared_ptr<WoWGameObject>>     m_WoWGameObject;
	std::map<CWoWObjectGuid, std::shared_ptr<WoWDynamicObject>>  m_WoWDynamicObjects;
	std::map<CWoWObjectGuid, std::shared_ptr<WoWCorpse>>         m_WoWCorpses;
};