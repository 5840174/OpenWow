#pragma once

enum ZN_API ItemFlags : uint32
{
	ITEM_FLAG_NO_PICKUPx00000001,
	ITEM_FLAG_CONJUREDx00000002, // Conjured item
	ITEM_FLAG_HAS_LOOTx00000004, // Item can be right clicked to open for loot
	ITEM_FLAG_HEROIC_TOOLTIPx00000008, // Makes green "Heroic" text appear on item
	ITEM_FLAG_DEPRECATEDx00000010, // Cannot equip or use
	ITEM_FLAG_NO_USER_DESTROYx00000020, // Item can not be destroyed, except by using spell (item can be reagent for spell)
	ITEM_FLAG_PLAYERCASTx00000040, // Item's spells are castable by players
	ITEM_FLAG_NO_EQUIP_COOLDOWNx00000080, // No default 30 seconds cooldown when equipped
	ITEM_FLAG_MULTI_LOOT_QUESTx00000100,
	ITEM_FLAG_IS_WRAPPERx00000200, // Item can wrap other items
	ITEM_FLAG_USES_RESOURCESx00000400,
	ITEM_FLAG_MULTI_DROPx00000800, // Looting this item does not remove it from available loot
	ITEM_FLAG_ITEM_PURCHASE_RECORDx00001000, // Item can be returned to vendor for its original cost (extended cost)
	ITEM_FLAG_PETITIONx00002000, // Item is guild or arena charter
	ITEM_FLAG_HAS_TEXTx00004000, // Only readable items have this (but not all)
	ITEM_FLAG_NO_DISENCHANTx00008000,
	ITEM_FLAG_REAL_DURATIONx00010000,
	ITEM_FLAG_NO_CREATORx00020000,
	ITEM_FLAG_IS_PROSPECTABLEx00040000, // Item can be prospected
	ITEM_FLAG_UNIQUE_EQUIPPABLEx00080000, // You can only equip one of these
	ITEM_FLAG_IGNORE_FOR_AURASx00100000,
	ITEM_FLAG_IGNORE_DEFAULT_ARENA_RESTRICTIONSx00200000, // Item can be used during arena match
	ITEM_FLAG_NO_DURABILITY_LOSSx00400000, // Some Thrown weapons have it (and only Thrown) but not all
	ITEM_FLAG_USE_WHEN_SHAPESHIFTEDx00800000, // Item can be used in shapeshift forms
	ITEM_FLAG_HAS_QUEST_GLOWx01000000,
	ITEM_FLAG_HIDE_UNUSABLE_RECIPEx02000000, // Profession recipes: can only be looted if you meet requirements and don't already know it
	ITEM_FLAG_NOT_USEABLE_IN_ARENAx04000000, // Item cannot be used in arena
	ITEM_FLAG_IS_BOUND_TO_ACCOUNTx08000000, // Item binds to account and can be sent only to your own characters
	ITEM_FLAG_NO_REAGENT_COSTx10000000, // Spell is cast ignoring reagents
	ITEM_FLAG_IS_MILLABLEx20000000, // Item can be milled
	ITEM_FLAG_REPORT_TO_GUILD_CHATx40000000,
	ITEM_FLAG_NO_PROGRESSIVE_LOOTx80000000
};

enum ZN_API ItemFlags2 : uint32
{
	ITEM_FLAG2_FACTION_HORDEx00000001,
	ITEM_FLAG2_FACTION_ALLIANCEx00000002,
	ITEM_FLAG2_DONT_IGNORE_BUY_PRICEx00000004, // when item uses extended cost, gold is also required
	ITEM_FLAG2_CLASSIFY_AS_CASTERx00000008,
	ITEM_FLAG2_CLASSIFY_AS_PHYSICALx00000010,
	ITEM_FLAG2_EVERYONE_CAN_ROLL_NEEDx00000020,
	ITEM_FLAG2_NO_TRADE_BIND_ON_ACQUIREx00000040,
	ITEM_FLAG2_CAN_TRADE_BIND_ON_ACQUIREx00000080,
	ITEM_FLAG2_CAN_ONLY_ROLL_GREEDx00000100,
	ITEM_FLAG2_CASTER_WEAPONx00000200,
	ITEM_FLAG2_DELETE_ON_LOGINx00000400,
	ITEM_FLAG2_INTERNAL_ITEMx00000800,
	ITEM_FLAG2_NO_VENDOR_VALUEx00001000,
	ITEM_FLAG2_SHOW_BEFORE_DISCOVEREDx00002000,
	ITEM_FLAG2_OVERRIDE_GOLD_COSTx00004000,
	ITEM_FLAG2_IGNORE_DEFAULT_RATED_BG_RESTRICTIONSx00008000,
	ITEM_FLAG2_NOT_USABLE_IN_RATED_BGx00010000,
	ITEM_FLAG2_BNET_ACCOUNT_TRADE_OKx00020000,
	ITEM_FLAG2_CONFIRM_BEFORE_USEx00040000,
	ITEM_FLAG2_REEVALUATE_BONDING_ON_TRANSFORMx00080000,
	ITEM_FLAG2_NO_TRANSFORM_ON_CHARGE_DEPLETIONx00100000,
	ITEM_FLAG2_NO_ALTER_ITEM_VISUALx00200000,
	ITEM_FLAG2_NO_SOURCE_FOR_ITEM_VISUALx00400000,
	ITEM_FLAG2_IGNORE_QUALITY_FOR_ITEM_VISUAL_SOURCEx00800000,
	ITEM_FLAG2_NO_DURABILITYx01000000,
	ITEM_FLAG2_ROLE_TANKx02000000,
	ITEM_FLAG2_ROLE_HEALERx04000000,
	ITEM_FLAG2_ROLE_DAMAGEx08000000,
	ITEM_FLAG2_CAN_DROP_IN_CHALLENGE_MODEx10000000,
	ITEM_FLAG2_NEVER_STACK_IN_LOOT_UIx20000000,
	ITEM_FLAG2_DISENCHANT_TO_LOOT_TABLEx40000000,
	ITEM_FLAG2_USED_IN_A_TRADESKILLx80000000
};

struct ZN_API ItemDamageData
{
	float DamageMin;
	float DamageMax;
	uint32 DamageType;
};

struct ZN_API ItemStatData
{
	uint32 ItemStatType;
	int32 ItemStatValue;
};

struct ZN_API ItemSpellData
{
	int32 SpellId = -1;
	uint32 SpellTrigger;
	int32 SpellCharges;
	int32 SpellCooldown = -1;
	uint32 SpellCategory;
	int32 SpellCategoryCooldown = -1;
};

struct ZN_API ItemSocketData
{
	uint32 Color;
	uint32 Content;
};

struct ZN_API SItemQueryResult
{
	static const uint8 MAX_ITEM_PROTO_DAMAGES = 2;                            // changed in 3.1.0
	static const uint8 MAX_ITEM_PROTO_SOCKETS = 3;
	static const uint8 MAX_ITEM_PROTO_SPELLS = 5;
	static const uint8 MAX_ITEM_PROTO_STATS = 10;
	static const uint8 MAX_SPELL_SCHOOL = 7;

	SItemQueryResult(uint32 EntryID)
		: entryID(EntryID)
	{}

	uint32 entryID;
	uint32 Class;
	uint32 SubClass;
	int32 SoundOverrideSubclass;
	std::string Name;
	uint32 DisplayInfoID;
	uint32 Quality;
	ItemFlags Flags;
	ItemFlags2 Flags2;
	int32 BuyPrice;
	uint32 SellPrice;
	uint32 InventoryType;
	uint32 AllowableClass;
	uint32 AllowableRace;
	uint32 ItemLevel;
	uint32 RequiredLevel;
	uint32 RequiredSkill;
	uint32 RequiredSkillRank;
	uint32 RequiredSpell;
	uint32 RequiredHonorRank;
	uint32 RequiredCityRank;
	uint32 RequiredReputationFaction;
	uint32 RequiredReputationRank;
	int32 MaxCount;
	int32 Stackable;
	uint32 ContainerSlots;
	uint32 StatsCount;
	ItemStatData ItemStat[MAX_ITEM_PROTO_STATS];
	uint32 ScalingStatDistribution;
	uint32 ScalingStatValue;
	ItemDamageData Damage[MAX_ITEM_PROTO_DAMAGES];
	uint32 Resistance[MAX_SPELL_SCHOOL] = { };
	uint32 Delay;
	uint32 AmmoType;
	float RangedModRange;
	ItemSpellData Spells[MAX_ITEM_PROTO_SPELLS];
	uint32 Bonding;
	std::string Description;
	uint32 PageText;
	uint32 LanguageID;
	uint32 PageMaterial;
	uint32 StartQuest;
	uint32 LockID;
	int32 Material;
	uint32 Sheath;
	int32 RandomProperty;
	int32 RandomSuffix;
	uint32 Block;
	uint32 ItemSet;
	uint32 MaxDurability;
	uint32 Area;
	uint32 Map;
	uint32 BagFamily;
	uint32 TotemCategory;
	ItemSocketData Socket[MAX_ITEM_PROTO_SOCKETS];
	uint32 SocketBonus;
	uint32 GemProperties;
	uint32 RequiredDisenchantSkill;
	float ArmorDamageModifier;
	uint32 Duration;
	uint32 ItemLimitCategory;
	uint32 HolidayId;

	void Load(IByteBuffer& Bytes)
	{
		Bytes >> Class;
		Bytes >> SubClass;
		Bytes >> SoundOverrideSubclass;
		Bytes >> Name;
		Bytes.seekRelative(1); // Name2
		Bytes.seekRelative(1); // Name3
		Bytes.seekRelative(1); // Name3
		Bytes >> DisplayInfoID;
		Bytes >> Quality;
		Bytes >> Flags;
		Bytes >> Flags2;
		Bytes >> BuyPrice;
		Bytes >> SellPrice;
		Bytes >> InventoryType;
		Bytes >> AllowableClass;
		Bytes >> AllowableRace;
		Bytes >> ItemLevel;
		Bytes >> RequiredLevel;
		Bytes >> RequiredSkill;
		Bytes >> RequiredSkillRank;
		Bytes >> RequiredSpell;
		Bytes >> RequiredHonorRank;
		Bytes >> RequiredCityRank;
		Bytes >> RequiredReputationFaction;
		Bytes >> RequiredReputationRank;
		Bytes >> MaxCount;
		Bytes >> Stackable;
		Bytes >> ContainerSlots;
		Bytes >> StatsCount;
		if (StatsCount > 0)
			Bytes.readBytes(ItemStat, sizeof(ItemStatData) * StatsCount /* BE CAREFULL !!! */);
		Bytes >> ScalingStatDistribution;
		Bytes >> ScalingStatValue;
		Bytes.readBytes(Damage, sizeof(ItemDamageData) * MAX_ITEM_PROTO_DAMAGES);
		Bytes.readBytes(Resistance, sizeof(uint32) * MAX_SPELL_SCHOOL);
		Bytes >> Delay;
		Bytes >> AmmoType;
		Bytes >> RangedModRange;
		Bytes.readBytes(Spells, sizeof(ItemSpellData) * MAX_ITEM_PROTO_SPELLS);
		Bytes >> Bonding;
		Bytes >> Description;
		Bytes >> PageText;
		Bytes >> LanguageID;
		Bytes >> PageMaterial;
		Bytes >> StartQuest;
		Bytes >> LockID;
		Bytes >> Material;
		Bytes >> Sheath;
		Bytes >> RandomProperty;
		Bytes >> RandomSuffix;
		Bytes >> Block;
		Bytes >> ItemSet;
		Bytes >> MaxDurability;
		Bytes >> Area;
		Bytes >> Map;
		Bytes >> BagFamily;
		Bytes >> TotemCategory;
		Bytes.readBytes(Socket, sizeof(ItemSocketData) * MAX_ITEM_PROTO_SOCKETS);
		Bytes >> SocketBonus;
		Bytes >> GemProperties;
		Bytes >> RequiredDisenchantSkill;
		Bytes >> ArmorDamageModifier;
		Bytes >> Duration;
		Bytes >> ItemLimitCategory;
		Bytes >> HolidayId;
	}

	void Save(IByteBuffer& Bytes)
	{
		Bytes << Class;
		Bytes << SubClass;
		Bytes << SoundOverrideSubclass;
		Bytes << Name;
		Bytes.writeDummy(1); // Name2
		Bytes.writeDummy(1); // Name3
		Bytes.writeDummy(1); // Name3
		Bytes << DisplayInfoID;
		Bytes << Quality;
		Bytes << Flags;
		Bytes << Flags2;
		Bytes << BuyPrice;
		Bytes << SellPrice;
		Bytes << InventoryType;
		Bytes << AllowableClass;
		Bytes << AllowableRace;
		Bytes << ItemLevel;
		Bytes << RequiredLevel;
		Bytes << RequiredSkill;
		Bytes << RequiredSkillRank;
		Bytes << RequiredSpell;
		Bytes << RequiredHonorRank;
		Bytes << RequiredCityRank;
		Bytes << RequiredReputationFaction;
		Bytes << RequiredReputationRank;
		Bytes << MaxCount;
		Bytes << Stackable;
		Bytes << ContainerSlots;
		Bytes << StatsCount;
		if (StatsCount > 0)
			Bytes.writeBytes(ItemStat, sizeof(ItemStatData) * StatsCount /* BE CAREFULL !!! */);
		Bytes << ScalingStatDistribution;
		Bytes << ScalingStatValue;
		Bytes.writeBytes(Damage, sizeof(ItemDamageData) * MAX_ITEM_PROTO_DAMAGES);
		Bytes.writeBytes(Resistance, sizeof(uint32) * MAX_SPELL_SCHOOL);
		Bytes << Delay;
		Bytes << AmmoType;
		Bytes << RangedModRange;
		Bytes.writeBytes(Spells, sizeof(ItemSpellData) * MAX_ITEM_PROTO_SPELLS);
		Bytes << Bonding;
		Bytes << Description;
		Bytes << PageText;
		Bytes << LanguageID;
		Bytes << PageMaterial;
		Bytes << StartQuest;
		Bytes << LockID;
		Bytes << Material;
		Bytes << Sheath;
		Bytes << RandomProperty;
		Bytes << RandomSuffix;
		Bytes << Block;
		Bytes << ItemSet;
		Bytes << MaxDurability;
		Bytes << Area;
		Bytes << Map;
		Bytes << BagFamily;
		Bytes << TotemCategory;
		Bytes.writeBytes(Socket, sizeof(ItemSocketData) * MAX_ITEM_PROTO_SOCKETS);
		Bytes << SocketBonus;
		Bytes << GemProperties;
		Bytes << RequiredDisenchantSkill;
		Bytes << ArmorDamageModifier;
		Bytes << Duration;
		Bytes << ItemLimitCategory;
		Bytes << HolidayId;
	}

	size_t GetSize() const
	{
		size_t size = 0;
		size += sizeof(Class);
		size += sizeof(SubClass);
		size += sizeof(SoundOverrideSubclass);
		size += (Name.length() + 1);
		size += 1;
		size += 1;
		size += 1;
		size += sizeof(DisplayInfoID);
		size += sizeof(Quality);
		size += sizeof(Flags);
		size += sizeof(Flags2);
		size += sizeof(BuyPrice);
		size += sizeof(SellPrice);
		size += sizeof(InventoryType);
		size += sizeof(AllowableClass);
		size += sizeof(AllowableRace);
		size += sizeof(ItemLevel);
		size += sizeof(RequiredLevel);
		size += sizeof(RequiredSkill);
		size += sizeof(RequiredSkillRank);
		size += sizeof(RequiredSpell);
		size += sizeof(RequiredHonorRank);
		size += sizeof(RequiredCityRank);
		size += sizeof(RequiredReputationFaction);
		size += sizeof(RequiredReputationRank);
		size += sizeof(MaxCount);
		size += sizeof(Stackable);
		size += sizeof(ContainerSlots);
		size += sizeof(StatsCount);
		size += sizeof(ItemStatData) * StatsCount /* BE CAREFULL !!! */;
		size += sizeof(ScalingStatDistribution);
		size += sizeof(ScalingStatValue);
		size += sizeof(ItemDamageData) * MAX_ITEM_PROTO_DAMAGES;
		size += sizeof(uint32) * MAX_SPELL_SCHOOL;
		size += sizeof(Delay);
		size += sizeof(AmmoType);
		size += sizeof(RangedModRange);
		size += sizeof(ItemSpellData) * MAX_ITEM_PROTO_SPELLS;
		size += sizeof(Bonding);
		size += (Description.length() + 1);
		size += sizeof(PageText);
		size += sizeof(LanguageID);
		size += sizeof(PageMaterial);
		size += sizeof(StartQuest);
		size += sizeof(LockID);
		size += sizeof(Material);
		size += sizeof(Sheath);
		size += sizeof(RandomProperty);
		size += sizeof(RandomSuffix);
		size += sizeof(Block);
		size += sizeof(ItemSet);
		size += sizeof(MaxDurability);
		size += sizeof(Area);
		size += sizeof(Map);
		size += sizeof(BagFamily);
		size += sizeof(TotemCategory);
		size += sizeof(ItemSocketData) * MAX_ITEM_PROTO_SOCKETS;
		size += sizeof(SocketBonus);
		size += sizeof(GemProperties);
		size += sizeof(RequiredDisenchantSkill);
		size += sizeof(ArmorDamageModifier);
		size += sizeof(Duration);
		size += sizeof(ItemLimitCategory);
		size += sizeof(HolidayId);

		return size;
	}
};


ZN_INTERFACE ZN_API IClientCacheItemResponseListener
{
	virtual ~IClientCacheItemResponseListener() {}

	virtual void OnTemplate(CowGuid::EntryType_t Entry, const std::shared_ptr<SItemQueryResult>& QueryResult) = 0;
};


#include "WDBFile.h"


class ZN_API CowWDBItemCache
	: public CowWDBFile
{
public:
	CowWDBItemCache(IFilesManager* FilesManager);
	virtual ~CowWDBItemCache();

	bool Get(CowGuid::EntryType_t Entry, std::shared_ptr<SItemQueryResult> * QueryResult) const;
	void Add(CowGuid::EntryType_t Entry, std::shared_ptr<SItemQueryResult> QueryResult);

protected:
	void CreateCacheBuffer(IByteBuffer * ByteBuffer) const override;

private:
	std::map<CowGuid::EntryType_t, std::shared_ptr<SItemQueryResult>> m_Cache;
};