#pragma once

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
	static const uint8 MAX_GAMEOBJECT_QUEST_ITEMS = 6;

	SGameObjectQueryResult(uint32 EntryID)
		: entryID(EntryID)
	{}

	uint32          entryID;
	GameobjectTypes GameObjectType;
	uint32          DisplayId;
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

	float Size;

	uint32 QuestItems[MAX_GAMEOBJECT_QUEST_ITEMS] = {};

	void Load(IByteBuffer& Bytes)
	{
		Bytes >> GameObjectType;
		Bytes >> DisplayId;

		Bytes >> Name;
		Bytes.seekRelative(1); // Name2
		Bytes.seekRelative(1); // Name3
		Bytes.seekRelative(1); // Name4
		Bytes >> IconName;
		Bytes >> CastBarCaption;
		Bytes >> UnkString;

		Bytes.readBytes(raw.data, sizeof(uint32) * 24);
		Bytes >> Size;
		Bytes.readBytes(QuestItems, sizeof(uint32) * MAX_GAMEOBJECT_QUEST_ITEMS);
	}

	void Save(IByteBuffer& Bytes) const
	{
		Bytes << GameObjectType;
		Bytes << DisplayId;

		Bytes << Name;
		Bytes.writeDummy(1); // Name2
		Bytes.writeDummy(1); // Name3
		Bytes.writeDummy(1); // Name4
		Bytes << IconName;
		Bytes << CastBarCaption;
		Bytes << UnkString;

		Bytes.writeBytes(raw.data, sizeof(uint32) * 24);
		Bytes << Size;
		Bytes.writeBytes(QuestItems, sizeof(uint32) * MAX_GAMEOBJECT_QUEST_ITEMS);
	}

	size_t GetSize() const
	{
		size_t size = 0;
		size += sizeof(GameObjectType);
		size += sizeof(DisplayId);
		size += (Name.length() + 1);
		size += 1;
		size += 1;
		size += 1;
		size += (IconName.length() + 1);
		size += (CastBarCaption.length() + 1);
		size += (UnkString.length() + 1);
		size += sizeof(raw.data);
		size += sizeof(Size);
		size += sizeof(QuestItems);
		return size;
	}
};


ZN_INTERFACE ZN_API IClientCacheGameobjectResponseListener
{
	virtual ~IClientCacheGameobjectResponseListener() {}

	virtual void OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SGameObjectQueryResult>& QueryResult) = 0;
};


#include "WDBFile.h"


class ZN_API CowWDBGameObjectCache
	: public CowWDBFile
{
public:
	CowWDBGameObjectCache(IFilesManager* FilesManager);
	virtual ~CowWDBGameObjectCache();

	bool Get(CowGuid::EntryType_t Entry, std::shared_ptr<SGameObjectQueryResult> * QueryResult) const;
	void Add(std::shared_ptr<SGameObjectQueryResult> QueryResult);

protected:
	void CreateCacheBuffer(IByteBuffer * ByteBuffer) const override;

private:
	std::map<CowGuid::EntryType_t, std::shared_ptr<SGameObjectQueryResult>> m_Cache;
};