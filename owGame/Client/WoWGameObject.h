#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "WoWWorldObject.h"

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

enum ZN_API GameobjectTypes : uint32
{
    GAMEOBJECT_TYPE_DOOR                   = 0,
    GAMEOBJECT_TYPE_BUTTON                 = 1,
    GAMEOBJECT_TYPE_QUESTGIVER             = 2,
    GAMEOBJECT_TYPE_CHEST                  = 3,
    GAMEOBJECT_TYPE_BINDER                 = 4,
    GAMEOBJECT_TYPE_GENERIC                = 5,
    GAMEOBJECT_TYPE_TRAP                   = 6,
    GAMEOBJECT_TYPE_CHAIR                  = 7,
    GAMEOBJECT_TYPE_SPELL_FOCUS            = 8,
    GAMEOBJECT_TYPE_TEXT                   = 9,
    GAMEOBJECT_TYPE_GOOBER                 = 10,
    GAMEOBJECT_TYPE_TRANSPORT              = 11,
    GAMEOBJECT_TYPE_AREADAMAGE             = 12,
    GAMEOBJECT_TYPE_CAMERA                 = 13,
    GAMEOBJECT_TYPE_MAP_OBJECT             = 14,
    GAMEOBJECT_TYPE_MO_TRANSPORT           = 15,
    GAMEOBJECT_TYPE_DUEL_ARBITER           = 16,
    GAMEOBJECT_TYPE_FISHINGNODE            = 17,
    GAMEOBJECT_TYPE_SUMMONING_RITUAL       = 18,
    GAMEOBJECT_TYPE_MAILBOX                = 19,
    GAMEOBJECT_TYPE_DO_NOT_USE             = 20,
    GAMEOBJECT_TYPE_GUARDPOST              = 21,
    GAMEOBJECT_TYPE_SPELLCASTER            = 22,
    GAMEOBJECT_TYPE_MEETINGSTONE           = 23,
    GAMEOBJECT_TYPE_FLAGSTAND              = 24,
    GAMEOBJECT_TYPE_FISHINGHOLE            = 25,
    GAMEOBJECT_TYPE_FLAGDROP               = 26,
    GAMEOBJECT_TYPE_MINI_GAME              = 27,
    GAMEOBJECT_TYPE_DO_NOT_USE_2           = 28,
    GAMEOBJECT_TYPE_CAPTURE_POINT          = 29,
    GAMEOBJECT_TYPE_AURA_GENERATOR         = 30,
    GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY     = 31,
    GAMEOBJECT_TYPE_BARBER_CHAIR           = 32,
    GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING  = 33,
    GAMEOBJECT_TYPE_GUILD_BANK             = 34,
    GAMEOBJECT_TYPE_TRAPDOOR               = 35
};

class ZN_API WoWGameObject
	: public WorldObject
{
public:
	WoWGameObject(IScene& Scene, ObjectGuid Guid);
	virtual ~WoWGameObject();

public:
	static std::shared_ptr<WoWGameObject> Create(IScene& Scene, ObjectGuid Guid);
	virtual void AfterCreate(IScene& Scene) override;
	virtual void Destroy() override;

private:
	std::shared_ptr<ISceneNode> m_HiddenNode;
};

#endif
