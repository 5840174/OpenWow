#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "../Base/WoWWorldObject.h"
#include "../../Client/WDB/WDBCreatureCache.h"
#include "../../Client/WDB/WDBGameObjectCache.h"

enum ZN_API GameObjectFlags
{
    GO_FLAG_IN_USE          = 0x00000001,                   // disables interaction while animated
    GO_FLAG_LOCKED          = 0x00000002,                   // require key, spell, event, etc to be opened. Makes "Locked" appear in tooltip
    GO_FLAG_INTERACT_COND   = 0x00000004,                   // cannot interact (condition to interact - requires GO_DYNFLAG_LO_ACTIVATE to enable interaction clientside)
    GO_FLAG_TRANSPORT       = 0x00000008,                   // any kind of transport? Object can transport (elevator, boat, car)
    GO_FLAG_NOT_SELECTABLE  = 0x00000010,                   // not selectable even in GM mode
    GO_FLAG_NODESPAWN       = 0x00000020,                   // never despawn, typically for doors, they just change state
    GO_FLAG_AI_OBSTACLE     = 0x00000040,                   // makes the client register the object in something called AIObstacleMgr, unknown what it does
    GO_FLAG_FREEZE_ANIMATION = 0x00000080,
    GO_FLAG_DAMAGED         = 0x00000200,
    GO_FLAG_DESTROYED       = 0x00000400
};

enum ZN_API GameObjectDynamicLowFlags
{
    GO_DYNFLAG_LO_ACTIVATE          = 0x01,                 // enables interaction with GO
    GO_DYNFLAG_LO_ANIMATE           = 0x02,                 // possibly more distinct animation of GO
    GO_DYNFLAG_LO_NO_INTERACT       = 0x04,                 // appears to disable interaction (not fully verified)
    GO_DYNFLAG_LO_SPARKLE           = 0x08,                 // makes GO sparkle
    GO_DYNFLAG_LO_STOPPED           = 0x10                  // Transport is stopped
};


class ZN_API CowServerGameObject
	: public CowServerWorldObject
	, public IClientCacheGameobjectResponseListener
{
public:
	CowServerGameObject(IScene& Scene, CowServerWorld& WoWWorld, CowGuid Guid);
	virtual ~CowServerGameObject();

	virtual void OnValueUpdated(uint16 index) override;
	virtual void OnValuesUpdated(const UpdateMask & Mask) override;

	// IClientCacheGameobjectResponseListener
	void OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult) override;

	// ISceneNode
	void Update(const UpdateEventArgs& e);

public:
	static std::shared_ptr<CowServerGameObject> Create(CowServerWorld& WoWWorld, IScene& Scene, CowGuid Guid);
	virtual void Destroy() override;

protected:
	virtual void OnDisplayIDChanged(uint32 DisplayID);

protected:
	std::shared_ptr<SGameObjectQueryResult> m_GameObjectTemplate;

	std::shared_ptr<ISceneNode> m_GameObjectModel;
};

#endif
