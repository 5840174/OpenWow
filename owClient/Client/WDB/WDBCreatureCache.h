#pragma once

enum ZN_API CreatureType : uint32   // CreatureType.dbc
{
	CREATURE_TYPE_BEAST = 1,
	CREATURE_TYPE_DRAGONKIN = 2,
	CREATURE_TYPE_DEMON = 3,
	CREATURE_TYPE_ELEMENTAL = 4,
	CREATURE_TYPE_GIANT = 5,
	CREATURE_TYPE_UNDEAD = 6,
	CREATURE_TYPE_HUMANOID = 7,
	CREATURE_TYPE_CRITTER = 8,
	CREATURE_TYPE_MECHANICAL = 9,
	CREATURE_TYPE_NOT_SPECIFIED = 10,
	CREATURE_TYPE_TOTEM = 11,
	CREATURE_TYPE_NON_COMBAT_PET = 12,
	CREATURE_TYPE_GAS_CLOUD = 13
};

enum ZN_API CreatureTypeFlags : uint32
{
	CREATURE_TYPE_FLAG_TAMEABLE_PET = 0x00000001,   // Makes the mob tameable (must also be a beast and have family set)
	CREATURE_TYPE_FLAG_GHOST_VISIBLE = 0x00000002,   // Creature are also visible for not alive player. Allow gossip interaction if npcflag allow?
	CREATURE_TYPE_FLAG_BOSS_MOB = 0x00000004,   // Changes creature's visible level to "??" in the creature's portrait - Immune Knockback.
	CREATURE_TYPE_FLAG_DO_NOT_PLAY_WOUND_PARRY_ANIMATION = 0x00000008,
	CREATURE_TYPE_FLAG_HIDE_FACTION_TOOLTIP = 0x00000010,
	CREATURE_TYPE_FLAG_UNK5 = 0x00000020,   // Sound related
	CREATURE_TYPE_FLAG_SPELL_ATTACKABLE = 0x00000040,
	CREATURE_TYPE_FLAG_CAN_INTERACT_WHILE_DEAD = 0x00000080,   // Player can interact with the creature if its dead (not player dead)
	CREATURE_TYPE_FLAG_HERB_SKINNING_SKILL = 0x00000100,   // Can be looted by herbalist
	CREATURE_TYPE_FLAG_MINING_SKINNING_SKILL = 0x00000200,   // Can be looted by miner
	CREATURE_TYPE_FLAG_DO_NOT_LOG_DEATH = 0x00000400,   // Death event will not show up in combat log
	CREATURE_TYPE_FLAG_MOUNTED_COMBAT_ALLOWED = 0x00000800,   // Creature can remain mounted when entering combat
	CREATURE_TYPE_FLAG_CAN_ASSIST = 0x00001000,   // ? Can aid any player in combat if in range?
	CREATURE_TYPE_FLAG_IS_PET_BAR_USED = 0x00002000,
	CREATURE_TYPE_FLAG_MASK_UID = 0x00004000,
	CREATURE_TYPE_FLAG_ENGINEERING_SKINNING_SKILL = 0x00008000,   // Can be looted by engineer
	CREATURE_TYPE_FLAG_EXOTIC_PET = 0x00010000,   // Can be tamed by hunter as exotic pet
	CREATURE_TYPE_FLAG_USE_DEFAULT_COLLISION_BOX = 0x00020000,   // Collision related. (always using default collision box?)
	CREATURE_TYPE_FLAG_IS_SIEGE_WEAPON = 0x00040000,
	CREATURE_TYPE_FLAG_CAN_COLLIDE_WITH_MISSILES = 0x00080000,   // Projectiles can collide with this creature - interacts with TARGET_DEST_TRAJ
	CREATURE_TYPE_FLAG_HIDE_NAME_PLATE = 0x00100000,
	CREATURE_TYPE_FLAG_DO_NOT_PLAY_MOUNTED_ANIMATIONS = 0x00200000,
	CREATURE_TYPE_FLAG_IS_LINK_ALL = 0x00400000,
	CREATURE_TYPE_FLAG_INTERACT_ONLY_WITH_CREATOR = 0x00800000,
	CREATURE_TYPE_FLAG_DO_NOT_PLAY_UNIT_EVENT_SOUNDS = 0x01000000,
	CREATURE_TYPE_FLAG_HAS_NO_SHADOW_BLOB = 0x02000000,
	CREATURE_TYPE_FLAG_TREAT_AS_RAID_UNIT = 0x04000000,   // ! Creature can be targeted by spells that require target to be in caster's party/raid
	CREATURE_TYPE_FLAG_FORCE_GOSSIP = 0x08000000,   // Allows the creature to display a single gossip option.
	CREATURE_TYPE_FLAG_DO_NOT_SHEATHE = 0x10000000,
	CREATURE_TYPE_FLAG_DO_NOT_TARGET_ON_INTERACTION = 0x20000000,
	CREATURE_TYPE_FLAG_DO_NOT_RENDER_OBJECT_NAME = 0x40000000,
	CREATURE_TYPE_FLAG_UNIT_IS_QUEST_BOSS = 0x80000000    // Not verified
};

enum ZN_API CreatureEliteType : uint32
{
	CREATURE_ELITE_NORMAL = 0,
	CREATURE_ELITE_ELITE = 1,
	CREATURE_ELITE_RAREELITE = 2,
	CREATURE_ELITE_WORLDBOSS = 3,
	CREATURE_ELITE_RARE = 4,
	CREATURE_UNKNOWN = 5                      // found in 2.2.3 for 2 mobs
};

enum ZN_API CreatureFamily : uint32
{
	CREATURE_FAMILY_NONE = 0,
	CREATURE_FAMILY_WOLF = 1,
	CREATURE_FAMILY_CAT = 2,
	CREATURE_FAMILY_SPIDER = 3,
	CREATURE_FAMILY_BEAR = 4,
	CREATURE_FAMILY_BOAR = 5,
	CREATURE_FAMILY_CROCOLISK = 6,
	CREATURE_FAMILY_CARRION_BIRD = 7,
	CREATURE_FAMILY_CRAB = 8,
	CREATURE_FAMILY_GORILLA = 9,
	CREATURE_FAMILY_HORSE_CUSTOM = 10,   // Does not exist in DBC but used for horse like beasts in DB
	CREATURE_FAMILY_RAPTOR = 11,
	CREATURE_FAMILY_TALLSTRIDER = 12,
	CREATURE_FAMILY_FELHUNTER = 15,
	CREATURE_FAMILY_VOIDWALKER = 16,
	CREATURE_FAMILY_SUCCUBUS = 17,
	CREATURE_FAMILY_DOOMGUARD = 19,
	CREATURE_FAMILY_SCORPID = 20,
	CREATURE_FAMILY_TURTLE = 21,
	CREATURE_FAMILY_IMP = 23,
	CREATURE_FAMILY_BAT = 24,
	CREATURE_FAMILY_HYENA = 25,
	CREATURE_FAMILY_BIRD_OF_PREY = 26,
	CREATURE_FAMILY_WIND_SERPENT = 27,
	CREATURE_FAMILY_REMOTE_CONTROL = 28,
	CREATURE_FAMILY_FELGUARD = 29,
	CREATURE_FAMILY_DRAGONHAWK = 30,
	CREATURE_FAMILY_RAVAGER = 31,
	CREATURE_FAMILY_WARP_STALKER = 32,
	CREATURE_FAMILY_SPOREBAT = 33,
	CREATURE_FAMILY_NETHER_RAY = 34,
	CREATURE_FAMILY_SERPENT = 35,
	CREATURE_FAMILY_MOTH = 37,
	CREATURE_FAMILY_CHIMAERA = 38,
	CREATURE_FAMILY_DEVILSAUR = 39,
	CREATURE_FAMILY_GHOUL = 40,
	CREATURE_FAMILY_SILITHID = 41,
	CREATURE_FAMILY_WORM = 42,
	CREATURE_FAMILY_RHINO = 43,
	CREATURE_FAMILY_WASP = 44,
	CREATURE_FAMILY_CORE_HOUND = 45,
	CREATURE_FAMILY_SPIRIT_BEAST = 46
};

struct ZN_API SCreatureQueryResult
{
	static const uint8 MAX_KILL_CREDIT = 2;
	static const uint32 MAX_CREATURE_MODELS = 4;
	static const uint32 MAX_CREATURE_QUEST_ITEMS = 6;

	SCreatureQueryResult(uint32 EntryID)
		: entry(EntryID)
	{}

	uint32 entry;

	std::string Name;
	std::string SubName;
	std::string CursorName;
	CreatureTypeFlags CreatureTypeFlags;
	CreatureType CreatureType;
	CreatureFamily CreatureFamily;
	CreatureEliteType CreatureEliteType;

	uint32 ProxyCreatureID[MAX_KILL_CREDIT] = { 0 }; // new in 3.1, kill credit
	uint32 CreatureDisplayID[MAX_CREATURE_MODELS] = { 0 }; // Modelid
	float HpMulti = 0.0f; // dmg/hp modifier
	float EnergyMulti = 0.0f; // dmg/mana modifier
	bool IsLeader = false;
	uint32 QuestItems[MAX_CREATURE_QUEST_ITEMS] = { 0 };
	uint32 CreatureMovementInfoID = 0;    // CreatureMovementInfo.dbc

	void Load(IByteBuffer& Bytes)
	{
		Bytes >> Name;
		Bytes.seekRelative(1); // Name2
		Bytes.seekRelative(1); // Name3
		Bytes.seekRelative(1); // Name3
		Bytes >> SubName;
		Bytes >> CursorName;

		Bytes >> CreatureTypeFlags;
		Bytes >> CreatureType;           // CreatureType.dbc
		Bytes >> CreatureFamily;         // CreatureFamily.dbc
		Bytes >> CreatureEliteType; // Creature Rank (elite, boss, etc)

		Bytes.readBytes(ProxyCreatureID, sizeof(uint32) * MAX_KILL_CREDIT);
		Bytes.readBytes(CreatureDisplayID, sizeof(uint32) * MAX_CREATURE_MODELS);

		Bytes >> HpMulti;
		Bytes >> EnergyMulti;

		Bytes >> IsLeader;

		Bytes.readBytes(QuestItems, sizeof(uint32) * MAX_CREATURE_QUEST_ITEMS);

		Bytes >> CreatureMovementInfoID;
	}

	void Save(IByteBuffer& Bytes) const
	{
		Bytes << Name;
		Bytes.writeDummy(1); // Name2
		Bytes.writeDummy(1); // Name3
		Bytes.writeDummy(1); // Name3
		Bytes << SubName;
		Bytes << CursorName;

		Bytes << CreatureTypeFlags;
		Bytes << CreatureType;           // CreatureType.dbc
		Bytes << CreatureFamily;         // CreatureFamily.dbc
		Bytes << CreatureEliteType; // Creature Rank (elite, boss, etc)

		Bytes.writeBytes(ProxyCreatureID, sizeof(uint32) * MAX_KILL_CREDIT);
		Bytes.writeBytes(CreatureDisplayID, sizeof(uint32) * MAX_CREATURE_MODELS);

		Bytes << HpMulti;
		Bytes << EnergyMulti;

		Bytes << IsLeader;

		Bytes.writeBytes(QuestItems, sizeof(uint32) * MAX_CREATURE_QUEST_ITEMS);

		Bytes << CreatureMovementInfoID;
	}

	size_t GetSize() const
	{
		size_t size = 0;

		size += (Name.length() + 1);
		size += 1;
		size += 1;
		size += 1;
		size += (SubName.length() + 1);
		size += (CursorName.length() + 1);

		size += sizeof(CreatureTypeFlags);
		size += sizeof(CreatureType);
		size += sizeof(CreatureFamily);
		size += sizeof(CreatureEliteType);

		size += sizeof(ProxyCreatureID);
		size += sizeof(CreatureDisplayID);

		size += sizeof(HpMulti);
		size += sizeof(EnergyMulti);

		size += sizeof(IsLeader);

		size += sizeof(QuestItems);
		size += sizeof(CreatureMovementInfoID);

		return size;
	}
};

ZN_INTERFACE ZN_API IClientCacheCreatureResponseListener
{
	virtual ~IClientCacheCreatureResponseListener() {}

	virtual void OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SCreatureQueryResult>& QueryResult) = 0;
};


#include "WDBFile.h"


class ZN_API CowWDBCreatureCache
	: public CowWDBFile
{
public:
	CowWDBCreatureCache(IFilesManager* FilesManager);
	virtual ~CowWDBCreatureCache();

	bool Get(CowGuid::EntryType_t Entry, std::shared_ptr<SCreatureQueryResult> * QueryResult) const;
	void Add(CowGuid::EntryType_t Entry, std::shared_ptr<SCreatureQueryResult> QueryResult);

protected:
	void CreateCacheBuffer(IByteBuffer * ByteBuffer) const override;

private:
	std::map<CowGuid::EntryType_t, std::shared_ptr<SCreatureQueryResult>> m_Cache;
};