#pragma once

#include "Player_Enums.h"
#include "ItemTemplate.h"

enum class Race : uint8
{
	Human = 1,
	Orc = 2,
	Dwarf = 3,
	Nightelf = 4,
	Undead = 5,
	Tauren = 6,
	Gnome = 7,
	Troll = 8,
	Goblin = 9,
#if WOW_CLIENT_VERSION >= WOW_BC_2_4_3
	Bloodelf = 10,
	Draenei = 11,
	FelOrc = 12,
	Naga = 13,
	Broken = 14,
	Skeleton = 15,
#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
	Vrykul = 16,
	Tuskarr = 17,
#endif
	ForestTroll = 18,
#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
	Taunka = 19,
	NorthrendSkeleton = 20,
	IceTroll = 21
#endif
#endif
};

enum class Class : uint8
{
	Warrior = 1,
	Paladin = 2,
	Hunter = 3,
	Rogue = 4,
	Priest = 5,
#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
	DeathKnight = 6,
#endif
	Shaman = 7,
	Mage = 8,
	Warlock = 9,
	Druid = 11,
};

enum class Gender : uint8
{
	Male = 0,
	Female = 1,
	None = 2,
};

enum CharacterFlags : uint32
{
	CHARACTER_FLAG_NONE = 0x00000000,
	CHARACTER_FLAG_UNK1 = 0x00000001,
	CHARACTER_FLAG_UNK2 = 0x00000002,
	CHARACTER_FLAG_LOCKED_FOR_TRANSFER = 0x00000004,
	CHARACTER_FLAG_UNK4 = 0x00000008,
	CHARACTER_FLAG_UNK5 = 0x00000010,
	CHARACTER_FLAG_UNK6 = 0x00000020,
	CHARACTER_FLAG_UNK7 = 0x00000040,
	CHARACTER_FLAG_UNK8 = 0x00000080,
	CHARACTER_FLAG_UNK9 = 0x00000100,
	CHARACTER_FLAG_UNK10 = 0x00000200,
	CHARACTER_FLAG_HIDE_HELM = 0x00000400,
	CHARACTER_FLAG_HIDE_CLOAK = 0x00000800,
	CHARACTER_FLAG_UNK13 = 0x00001000,
	CHARACTER_FLAG_GHOST = 0x00002000,
	CHARACTER_FLAG_RENAME = 0x00004000,
	CHARACTER_FLAG_UNK16 = 0x00008000,
	CHARACTER_FLAG_UNK17 = 0x00010000,
	CHARACTER_FLAG_UNK18 = 0x00020000,
	CHARACTER_FLAG_UNK19 = 0x00040000,
	CHARACTER_FLAG_UNK20 = 0x00080000,
	CHARACTER_FLAG_UNK21 = 0x00100000,
	CHARACTER_FLAG_UNK22 = 0x00200000,
	CHARACTER_FLAG_UNK23 = 0x00400000,
	CHARACTER_FLAG_UNK24 = 0x00800000,
	CHARACTER_FLAG_LOCKED_BY_BILLING = 0x01000000,
	CHARACTER_FLAG_DECLINED = 0x02000000,
	CHARACTER_FLAG_UNK27 = 0x04000000,
	CHARACTER_FLAG_UNK28 = 0x08000000,
	CHARACTER_FLAG_UNK29 = 0x10000000,
	CHARACTER_FLAG_UNK30 = 0x20000000,
	CHARACTER_FLAG_UNK31 = 0x40000000,
	CHARACTER_FLAG_UNK32 = 0x80000000
};

enum CharacterCustomizeFlags : uint32
{
	CHAR_CUSTOMIZE_FLAG_NONE = 0x00000000,
	CHAR_CUSTOMIZE_FLAG_CUSTOMIZE = 0x00000001,       // name, gender, etc...
	CHAR_CUSTOMIZE_FLAG_FACTION = 0x00010000,       // name, gender, faction, etc...
	CHAR_CUSTOMIZE_FLAG_RACE = 0x00100000        // name, gender, race, etc...
};


#pragma pack(push, 1)

struct ZN_API CInet_CharacterTemplate
{
	CInet_CharacterTemplate()
		// System
		: GUID(0)
		, Name("Default name")
		// Race
		, Race(Race::Human)
		, Class(Class::Warrior)
		, Gender(Gender::Male)
		// Style
		, skin(0)
		, face(0)
		, hairStyle(0)
		, hairColor(0)
		, facialStyle(0)
		// Level
		, Level(255)
		// Location
		, ZoneId(0)
		, MapId(0)
		, Position(0.0f)
		// Guild
		, GuildId(0)
		// Flags
		, Flags(CharacterFlags::CHARACTER_FLAG_NONE)
		, IsFirstLogin(true)
		// Pet
		, PetInfoId(0)
		, PetLevel(0)
		, PetFamilyId(0)
	{
		for (int i = 0; i < INVENTORY_SLOT_BAG_END; i++)
			ItemsTemplates[i] = CInet_ItemTemplate();
	}

	CInet_CharacterTemplate(CByteBuffer& b)
	{
		b >> GUID;
		b >> Name;
		b >> Race;
		b >> Class;
		b >> Gender;

		b >> skin;
		b >> face;
		b >> hairStyle;
		b >> hairColor;
		b >> facialStyle;

		b >> Level;

		b >> ZoneId;
		b >> MapId;
		b >> Position;

		b >> GuildId;

		b >> Flags;
		b >> CustomizationFlags;

		b >> IsFirstLogin;

		b >> PetInfoId;
		b >> PetLevel;
		b >> PetFamilyId;

		for (int i = 0; i < INVENTORY_SLOT_BAG_END; i++)
			ItemsTemplates[i] = CInet_ItemTemplate(b);
	}

	void TemplatePrint()
	{
		Log::Info("Char %s, -------------------------------------------------------------", Name.c_str());
		Log::Info("Lvl %d, Race %d, Class %d, Gender %d", Level, Race, Class, Gender);
		Log::Info("Zone '%d', Map '%d', Pos(%f, %f, %f)", ZoneId, MapId, Position.x, Position.y, Position.z);
		Log::Info("skin '%d', face '%d', hairStyle '%d', hairColor '%d', facialStyle '%d'", skin, face, hairStyle, hairColor, facialStyle);
	}

	// System
	uint64						GUID;
	std::string					Name;
	// Race
	Race					    Race;
	Class					    Class;
	Gender				        Gender;
	// Style
	uint8						skin;
	uint8						face;
	uint8						hairStyle;
	uint8						hairColor;
	uint8						facialStyle;
	// Level
	uint8						Level;
	// Location
	uint32						ZoneId;
	uint32						MapId;
	glm::vec3                   Position;

	// Guild
	uint32						GuildId;
	// Flags
	CharacterFlags				Flags;
	CharacterCustomizeFlags     CustomizationFlags;
	uint8						IsFirstLogin; // == 1 or == 0

	// Pet
	uint32						PetInfoId;
	uint32						PetLevel;
	uint32						PetFamilyId;

	// Items
	CInet_ItemTemplate			ItemsTemplates[INVENTORY_SLOT_BAG_END];
};

#pragma pack(pop)