#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWWorldObject.h"
#include "World/GameObject/GameObject.h"

enum ZN_API GameObjectFlags
{
	GO_FLAG_IN_USE = 0x00000001,                   //disables interaction while animated
	GO_FLAG_LOCKED = 0x00000002,                   //require key, spell, event, etc to be opened. Makes "Locked" appear in tooltip
	GO_FLAG_INTERACT_COND = 0x00000004,                   //cannot interact (condition to interact)
	GO_FLAG_TRANSPORT = 0x00000008,                   //any kind of transport? Object can transport (elevator, boat, car)
	GO_FLAG_UNK1 = 0x00000010,                   //
	GO_FLAG_NODESPAWN = 0x00000020,                   //never despawn, typically for doors, they just change state
	GO_FLAG_TRIGGERED = 0x00000040                    //typically, summoned objects. Triggered by spell or other events
};

enum ZN_API GameobjectTypes : uint32
{
	GAMEOBJECT_TYPE_DOOR = 0,
	GAMEOBJECT_TYPE_BUTTON = 1,
	GAMEOBJECT_TYPE_QUESTGIVER = 2,
	GAMEOBJECT_TYPE_CHEST = 3,
	GAMEOBJECT_TYPE_BINDER = 4,
	GAMEOBJECT_TYPE_GENERIC = 5,
	GAMEOBJECT_TYPE_TRAP = 6,
	GAMEOBJECT_TYPE_CHAIR = 7,
	GAMEOBJECT_TYPE_SPELL_FOCUS = 8,
	GAMEOBJECT_TYPE_TEXT = 9,
	GAMEOBJECT_TYPE_GOOBER = 10,
	GAMEOBJECT_TYPE_TRANSPORT = 11,
	GAMEOBJECT_TYPE_AREADAMAGE = 12,
	GAMEOBJECT_TYPE_CAMERA = 13,
	GAMEOBJECT_TYPE_MAP_OBJECT = 14,
	GAMEOBJECT_TYPE_MO_TRANSPORT = 15,
	GAMEOBJECT_TYPE_DUEL_ARBITER = 16,
	GAMEOBJECT_TYPE_FISHINGNODE = 17,
	GAMEOBJECT_TYPE_SUMMONING_RITUAL = 18,
	GAMEOBJECT_TYPE_MAILBOX = 19,
	GAMEOBJECT_TYPE_AUCTIONHOUSE = 20,
	GAMEOBJECT_TYPE_GUARDPOST = 21,
	GAMEOBJECT_TYPE_SPELLCASTER = 22,
	GAMEOBJECT_TYPE_MEETINGSTONE = 23,
	GAMEOBJECT_TYPE_FLAGSTAND = 24,
	GAMEOBJECT_TYPE_FISHINGHOLE = 25,
	GAMEOBJECT_TYPE_FLAGDROP = 26,
	GAMEOBJECT_TYPE_MINI_GAME = 27,

	//[TZERO] tbc enumerations [?]
	GAMEOBJECT_TYPE_LOTTERY_KIOSK = 28,
	GAMEOBJECT_TYPE_CAPTURE_POINT = 29,
	GAMEOBJECT_TYPE_AURA_GENERATOR = 30,
};

class ZN_API WoWGameObject
	: public WorldObject
{
public:
	WoWGameObject(IScene& Scene, ObjectGuid Guid);
	virtual ~WoWGameObject();

public:
	static std::shared_ptr<WoWGameObject> Create(IScene& Scene, ObjectGuid Guid);
	virtual void AfterCreate(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene * Scene);

private:
	std::shared_ptr<GameObject> m_HiddenNode;
};

#endif
