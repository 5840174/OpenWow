#pragma once


#define MAX_GAMEOBJECT_QUEST_ITEMS 6

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
	GAMEOBJECT_TYPE_DO_NOT_USE = 20,
	GAMEOBJECT_TYPE_GUARDPOST = 21,
	GAMEOBJECT_TYPE_SPELLCASTER = 22,
	GAMEOBJECT_TYPE_MEETINGSTONE = 23,
	GAMEOBJECT_TYPE_FLAGSTAND = 24,
	GAMEOBJECT_TYPE_FISHINGHOLE = 25,
	GAMEOBJECT_TYPE_FLAGDROP = 26,
	GAMEOBJECT_TYPE_MINI_GAME = 27,
	GAMEOBJECT_TYPE_DO_NOT_USE_2 = 28,
	GAMEOBJECT_TYPE_CAPTURE_POINT = 29,
	GAMEOBJECT_TYPE_AURA_GENERATOR = 30,
	GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY = 31,
	GAMEOBJECT_TYPE_BARBER_CHAIR = 32,
	GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING = 33,
	GAMEOBJECT_TYPE_GUILD_BANK = 34,
	GAMEOBJECT_TYPE_TRAPDOOR = 35
};

struct ZN_API SGameObjectQueryResult
{
	SGameObjectQueryResult(uint32 EntryID)
		: entryID(EntryID)
	{}

	uint32          entryID;
	GameobjectTypes type;
	uint32          displayId;
	std::string     Name;
	std::string     IconName;
	std::string     CastBarCaption;
	std::string     UnkString;

	union                                                   // different GO types have different data field
	{
		//0 GAMEOBJECT_TYPE_DOOR
		struct
		{
			uint32 startOpen;                               //0 used client side to determine GO_ACTIVATED means open/closed
			uint32 lockId;                                  //1 -> Lock.dbc
			uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
			uint32 noDamageImmune;                          //3 break opening whenever you recieve damage?
			uint32 openTextID;                              //4 can be used to replace castBarCaption?
			uint32 closeTextID;                             //5
			uint32 ignoredByPathing;                        //6
			uint32 conditionID1;                            //7
		} door;
		//1 GAMEOBJECT_TYPE_BUTTON
		struct
		{
			uint32 startOpen;                               //0
			uint32 lockId;                                  //1 -> Lock.dbc
			uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
			uint32 linkedTrap;                              //3
			uint32 noDamageImmune;                          //4 isBattlegroundObject
			uint32 large;                                   //5
			uint32 openTextID;                              //6 can be used to replace castBarCaption?
			uint32 closeTextID;                             //7
			uint32 losOK;                                   //8
			uint32 conditionID1;                            //9
		} button;
		//2 GAMEOBJECT_TYPE_QUESTGIVER
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 questList;                               //1
			uint32 pageMaterial;                            //2
			uint32 gossipID;                                //3
			uint32 customAnim;                              //4
			uint32 noDamageImmune;                          //5
			uint32 openTextID;                              //6 can be used to replace castBarCaption?
			uint32 losOK;                                   //7
			uint32 allowMounted;                            //8 Is usable while on mount/vehicle. (0/1)
			uint32 large;                                   //9
			uint32 conditionID1;                            //10
		} questgiver;
		//3 GAMEOBJECT_TYPE_CHEST
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 lootId;                                  //1
			uint32 chestRestockTime;                        //2
			uint32 consumable;                              //3
			uint32 minSuccessOpens;                         //4 Deprecated, pre 3.0 was used for mining nodes but since WotLK all mining nodes are usable once and grant all loot with a single use
			uint32 maxSuccessOpens;                         //5 Deprecated, pre 3.0 was used for mining nodes but since WotLK all mining nodes are usable once and grant all loot with a single use
			uint32 eventId;                                 //6 lootedEvent
			uint32 linkedTrapId;                            //7
			uint32 questId;                                 //8 not used currently but store quest required for GO activation for player
			uint32 level;                                   //9
			uint32 losOK;                                   //10
			uint32 leaveLoot;                               //11
			uint32 notInCombat;                             //12
			uint32 logLoot;                                 //13
			uint32 openTextID;                              //14 can be used to replace castBarCaption?
			uint32 groupLootRules;                          //15
			uint32 floatingTooltip;                         //16
			uint32 conditionID1;                            //17
		} chest;
		//4 GAMEOBJECT_TYPE_BINDER - empty
		//5 GAMEOBJECT_TYPE_GENERIC
		struct
		{
			uint32 floatingTooltip;                         //0
			uint32 highlight;                               //1
			uint32 serverOnly;                              //2
			uint32 large;                                   //3
			uint32 floatOnWater;                            //4
			int32 questID;                                  //5
			uint32 conditionID1;                            //6
		} _generic;
		//6 GAMEOBJECT_TYPE_TRAP
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 level;                                   //1
			uint32 diameter;                                //2 diameter for trap activation
			uint32 spellId;                                 //3
			uint32 type;                                    //4 0 trap with no despawn after cast. 1 trap despawns after cast. 2 bomb casts on spawn.
			uint32 cooldown;                                //5 time in secs
			int32 autoCloseTime;                            //6
			uint32 startDelay;                              //7
			uint32 serverOnly;                              //8
			uint32 stealthed;                               //9
			uint32 large;                                   //10
			uint32 invisible;                               //11
			uint32 openTextID;                              //12 can be used to replace castBarCaption?
			uint32 closeTextID;                             //13
			uint32 ignoreTotems;                            //14
			uint32 conditionID1;                            //15
		} trap;
		//7 GAMEOBJECT_TYPE_CHAIR
		struct
		{
			uint32 slots;                                   //0
			uint32 height;                                  //1
			uint32 onlyCreatorUse;                          //2
			uint32 triggeredEvent;                          //3
			uint32 conditionID1;                            //4
		} chair;
		//8 GAMEOBJECT_TYPE_SPELL_FOCUS
		struct
		{
			uint32 focusId;                                 //0
			uint32 dist;                                    //1
			uint32 linkedTrapId;                            //2
			uint32 serverOnly;                              //3
			uint32 questID;                                 //4
			uint32 large;                                   //5
			uint32 floatingTooltip;                         //6
			uint32 floatOnWater;                            //7
			uint32 conditionID1;                            //8
		} spellFocus;
		//9 GAMEOBJECT_TYPE_TEXT
		struct
		{
			uint32 pageID;                                  //0
			uint32 language;                                //1
			uint32 pageMaterial;                            //2
			uint32 allowMounted;                            //3 Is usable while on mount/vehicle. (0/1)
			uint32 conditionID1;                            //4
		} text;
		//10 GAMEOBJECT_TYPE_GOOBER
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			int32 questId;                                  //1
			uint32 eventId;                                 //2
			uint32 autoCloseTime;                           //3
			uint32 customAnim;                              //4
			uint32 consumable;                              //5
			uint32 cooldown;                                //6
			uint32 pageId;                                  //7
			uint32 language;                                //8
			uint32 pageMaterial;                            //9
			uint32 spellId;                                 //10
			uint32 noDamageImmune;                          //11
			uint32 linkedTrapId;                            //12
			uint32 large;                                   //13
			uint32 openTextID;                              //14 can be used to replace castBarCaption?
			uint32 closeTextID;                             //15
			uint32 losOK;                                   //16 isBattlegroundObject
			uint32 allowMounted;                            //17 Is usable while on mount/vehicle. (0/1)
			uint32 floatingTooltip;                         //18
			uint32 gossipID;                                //19
			uint32 WorldStateSetsState;                     //20
			uint32 floatOnWater;                            //21
			uint32 conditionID1;                            //22
		} goober;
		//11 GAMEOBJECT_TYPE_TRANSPORT
		struct
		{
			uint32 pause;                                   //0
			uint32 startOpen;                               //1
			uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
			uint32 pause1EventID;                           //3
			uint32 pause2EventID;                           //4
			uint32 mapID;                                   //5
		} transport;
		//12 GAMEOBJECT_TYPE_AREADAMAGE
		struct
		{
			uint32 lockId;                                  //0
			uint32 radius;                                  //1
			uint32 damageMin;                               //2
			uint32 damageMax;                               //3
			uint32 damageSchool;                            //4
			uint32 autoCloseTime;                           //5 secs till autoclose = autoCloseTime / 0x10000
			uint32 openTextID;                              //6
			uint32 closeTextID;                             //7
		} areadamage;
		//13 GAMEOBJECT_TYPE_CAMERA
		struct
		{
			uint32 lockId;                                  //0 -> Lock.dbc
			uint32 cinematicId;                             //1
			uint32 eventID;                                 //2
			uint32 openTextID;                              //3 can be used to replace castBarCaption?
			uint32 conditionID1;                            //4
		} camera;
		//14 GAMEOBJECT_TYPE_MAPOBJECT - empty
		//15 GAMEOBJECT_TYPE_MO_TRANSPORT
		struct
		{
			uint32 taxiPathId;                              //0
			uint32 moveSpeed;                               //1
			uint32 accelRate;                               //2
			uint32 startEventID;                            //3
			uint32 stopEventID;                             //4
			uint32 transportPhysics;                        //5
			uint32 mapID;                                   //6
			uint32 worldState1;                             //7
			uint32 canBeStopped;                            //8
		} moTransport;
		//16 GAMEOBJECT_TYPE_DUELFLAG - empty
		//17 GAMEOBJECT_TYPE_FISHINGNODE - empty
		//18 GAMEOBJECT_TYPE_SUMMONING_RITUAL
		struct
		{
			uint32 reqParticipants;                         //0
			uint32 spellId;                                 //1
			uint32 animSpell;                               //2
			uint32 ritualPersistent;                        //3
			uint32 casterTargetSpell;                       //4
			uint32 casterTargetSpellTargets;                //5
			uint32 castersGrouped;                          //6
			uint32 ritualNoTargetCheck;                     //7
			uint32 conditionID1;                            //8
		} summoningRitual;
		//19 GAMEOBJECT_TYPE_MAILBOX
		struct
		{
			uint32 conditionID1;                            //0
		} mailbox;
		//20 GAMEOBJECT_TYPE_DO_NOT_USE - empty
		//21 GAMEOBJECT_TYPE_GUARDPOST
		struct
		{
			uint32 creatureID;                              //0
			uint32 charges;                                 //1
		} guardpost;
		//22 GAMEOBJECT_TYPE_SPELLCASTER
		struct
		{
			uint32 spellId;                                 //0
			uint32 charges;                                 //1
			uint32 partyOnly;                               //2
			uint32 allowMounted;                            //3 Is usable while on mount/vehicle. (0/1)
			uint32 large;                                   //4
			uint32 conditionID1;                            //5
		} spellcaster;
		//23 GAMEOBJECT_TYPE_MEETINGSTONE
		struct
		{
			uint32 minLevel;                                //0
			uint32 maxLevel;                                //1
			uint32 areaID;                                  //2
		} meetingstone;
		//24 GAMEOBJECT_TYPE_FLAGSTAND
		struct
		{
			uint32 lockId;                                  //0
			uint32 pickupSpell;                             //1
			uint32 radius;                                  //2
			uint32 returnAura;                              //3
			uint32 returnSpell;                             //4
			uint32 noDamageImmune;                          //5
			uint32 openTextID;                              //6
			uint32 losOK;                                   //7
			uint32 conditionID1;                            //8
		} flagstand;
		//25 GAMEOBJECT_TYPE_FISHINGHOLE
		struct
		{
			uint32 radius;                                  //0 how close bobber must land for sending loot
			uint32 lootId;                                  //1
			uint32 minSuccessOpens;                         //2
			uint32 maxSuccessOpens;                         //3
			uint32 lockId;                                  //4 -> Lock.dbc; possibly 1628 for all?
		} fishinghole;
		//26 GAMEOBJECT_TYPE_FLAGDROP
		struct
		{
			uint32 lockId;                                  //0
			uint32 eventID;                                 //1
			uint32 pickupSpell;                             //2
			uint32 noDamageImmune;                          //3
			uint32 openTextID;                              //4
		} flagdrop;
		//27 GAMEOBJECT_TYPE_MINI_GAME
		struct
		{
			uint32 gameType;                                //0
		} miniGame;
		//28 GAMEOBJECT_TYPE_DO_NOT_USE_2 - empty
		//29 GAMEOBJECT_TYPE_CAPTURE_POINT
		struct
		{
			uint32 radius;                                  //0
			uint32 spell;                                   //1
			uint32 worldState1;                             //2
			uint32 worldstate2;                             //3
			uint32 winEventID1;                             //4
			uint32 winEventID2;                             //5
			uint32 contestedEventID1;                       //6
			uint32 contestedEventID2;                       //7
			uint32 progressEventID1;                        //8
			uint32 progressEventID2;                        //9
			uint32 neutralEventID1;                         //10
			uint32 neutralEventID2;                         //11
			uint32 neutralPercent;                          //12
			uint32 worldstate3;                             //13
			uint32 minSuperiority;                          //14
			uint32 maxSuperiority;                          //15
			uint32 minTime;                                 //16
			uint32 maxTime;                                 //17
			uint32 large;                                   //18
			uint32 highlight;                               //19
			uint32 startingValue;                           //20
			uint32 unidirectional;                          //21
		} capturePoint;
		//30 GAMEOBJECT_TYPE_AURA_GENERATOR
		struct
		{
			uint32 startOpen;                               //0
			uint32 radius;                                  //1
			uint32 auraID1;                                 //2
			uint32 conditionID1;                            //3
			uint32 auraID2;                                 //4
			uint32 conditionID2;                            //5
			uint32 serverOnly;                              //6
		} auraGenerator;
		//31 GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY
		struct
		{
			uint32 mapID;                                   //0
			uint32 difficulty;                              //1
		} dungeonDifficulty;
		//32 GAMEOBJECT_TYPE_BARBER_CHAIR
		struct
		{
			uint32 chairheight;                             //0
			uint32 heightOffset;                            //1
		} barberChair;
		//33 GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING
		struct
		{
			uint32 intactNumHits;                           //0
			uint32 creditProxyCreature;                     //1
			uint32 empty1;                                  //2
			uint32 intactEvent;                             //3
			uint32 empty2;                                  //4
			uint32 damagedNumHits;                          //5
			uint32 empty3;                                  //6
			uint32 empty4;                                  //7
			uint32 empty5;                                  //8
			uint32 damagedEvent;                            //9
			uint32 empty6;                                  //10
			uint32 empty7;                                  //11
			uint32 empty8;                                  //12
			uint32 empty9;                                  //13
			uint32 destroyedEvent;                          //14
			uint32 empty10;                                 //15
			uint32 rebuildingTimeSecs;                      //16
			uint32 empty11;                                 //17
			uint32 destructibleData;                        //18
			uint32 rebuildingEvent;                         //19
			uint32 empty12;                                 //20
			uint32 empty13;                                 //21
			uint32 damageEvent;                             //22
			uint32 empty14;                                 //23
		} building;
		//34 GAMEOBJECT_TYPE_GUILDBANK
		struct
		{
			uint32 conditionID1;                            //0
		} guildbank;
		//35 GAMEOBJECT_TYPE_TRAPDOOR
		struct
		{
			uint32 whenToPause;                             // 0
			uint32 startOpen;                               // 1
			uint32 autoClose;                               // 2
		} trapDoor;

		// not use for specific field access (only for output with loop by all filed), also this determinate max union size
		struct                                              // GAMEOBJECT_TYPE_SPELLCASTER
		{
			uint32 data[24];
		} raw;
	};

	uint32 QuestItems[MAX_GAMEOBJECT_QUEST_ITEMS] = {};

	void Fill(CByteBuffer& Bytes)
	{
		//Bytes >> entryID;
		Bytes >> type;
		Bytes >> displayId;
		Bytes >> Name;
		Bytes.seekRelative(1); // Name2
		Bytes.seekRelative(1); // Name3
		Bytes.seekRelative(1); // Name4

		Bytes >> IconName;
		Bytes >> CastBarCaption;
		Bytes >> UnkString;

		Bytes >> raw.data;

		float Size;
		Bytes >> Size;

		Bytes >> QuestItems;
	}

	void Print()
	{
		//Log::Info("GameObject query: EntryID '%d', Type '%d', Name '%s'", entryID, type, Name.c_str());
	}
};

ZN_INTERFACE ZN_API IClientCacheGameobjectResponseListener
{
	virtual ~IClientCacheGameobjectResponseListener() {}

	virtual void OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult) = 0;
};





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

static const uint8 MAX_KILL_CREDIT = 2;
static const uint32 MAX_CREATURE_MODELS = 4;
static const uint32 MAX_CREATURE_QUEST_ITEMS = 6;
static const uint32 MAX_CREATURE_SPELLS = 8;

struct ZN_API SCreatureQueryResult
{
	SCreatureQueryResult(uint32 EntryID)
		: entry(EntryID)
	{}

	uint32 entry;                          // creature entry
	std::string Name;
	std::string SubName;
	std::string CursorName;
	CreatureTypeFlags type_flags;          // flags          wdbFeild7=wad flags1
	CreatureType type;
	CreatureFamily family;                 // family         wdbFeild9
	CreatureEliteType Classification;                           // rank           wdbFeild10

	uint32 ProxyCreatureID[MAX_KILL_CREDIT] = { }; // new in 3.1, kill credit
	uint32 CreatureDisplayID[MAX_CREATURE_MODELS] = { }; // Modelid
	float HpMulti = 0.0f; // dmg/hp modifier
	float EnergyMulti = 0.0f; // dmg/mana modifier
	bool Leader = false;
	uint32 QuestItems[MAX_CREATURE_QUEST_ITEMS] = { };
	uint32 CreatureMovementInfoID = 0;    // CreatureMovementInfo.dbc

	void Fill(CByteBuffer& Bytes)
	{
		//Bytes >> entry;
		Bytes >> Name;
		Bytes.seekRelative(1); // Name2
		Bytes.seekRelative(1); // Name3
		Bytes.seekRelative(1); // Name3
		Bytes >> SubName;
		Bytes >> CursorName;

		Bytes >> type_flags;
		Bytes >> type;           // CreatureType.dbc
		Bytes >> family;         // CreatureFamily.dbc
		Bytes >> Classification; // Creature Rank (elite, boss, etc)

		Bytes >> ProxyCreatureID;
		Bytes >> CreatureDisplayID;

		Bytes >> HpMulti;
		Bytes >> EnergyMulti;

		Bytes >> Leader;

		Bytes >> QuestItems;

		Bytes >> CreatureMovementInfoID;
	}

	void Print()
	{
		//Log::Info("SCreatureQueryResult: ID '%d', Name '%s', SubName '%s'", entry, Name.c_str(), SubName.c_str());
	}
};

ZN_INTERFACE ZN_API IClientCacheCreatureResponseListener
{
	virtual ~IClientCacheCreatureResponseListener() {}

	virtual void OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SCreatureQueryResult>& QueryResult) = 0;
};