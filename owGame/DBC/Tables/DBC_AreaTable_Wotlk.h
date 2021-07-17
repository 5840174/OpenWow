#pragma once

#include "DBC/DBC__File.h"


enum class EAreaFlags : uint32
{
	HasBreathParticles = 0x00000001,           // snow (only Dun Morogh, Naxxramas, Razorfen Downs and Winterspring)
	BreathParticlesOverrideParent = 0x00000002,
	UNK2 = 0x00000004,           // Only used for areas on map 571 (development before)
	SLAVE_CAPITAL = 0x00000008,           // city and city subsones


	UNK3 = 0x00000010,           // can't find common meaning
	SLAVE_CAPITAL2 = 0x00000020,           // slave capital city flag?
	ALLOW_DUELS = 0x00000040,
	Arena = 0x00000080,           // arena, both instanced and world arenas


	CAPITAL = 0x00000100,           // main capital city flag
	LINKED_CHAT_AREA = 0x00000200,           // only one, hidden area for linked capital chat
	OUTLAND = 0x00000400,           // expansion zones? (only EOTS. Expansion azeroth zones like silvermyst island do not have this flag, 0x00004000 flag).
	//Flag determines if flying is allowed in zone.
	Sanctuary = 0x00000800,           // sanctuary area (PvP disabled)


	NEED_FLY = 0x00001000,           // only Netherwing Ledge, Socrethar's Seat, Tempest Keep, The Arcatraz, The Botanica, The Mechanar, Sorrow Wing Point, Dragonspine Ridge
	//Netherwing Mines, Dragonmaw Base Camp, Dragonmaw Skyway
	apply_ambient_multiplier_to_player = 0x00002000,           // not used now (no area/zones with this flag set in 3.0.3)
	OUTLAND2 = 0x00004000,           // expansion zones? (only Circle of Blood Arena not have this flag, but have 0x00000400 flag)
	IsSubZonePVPPOI = 0x00008000,           // pvp objective area? (Death's Door also has this flag although it's no pvp object area)


	ARENA_INSTANCE = 0x00010000,           // used by instanced arenas only
	UNUSED2 = 0x00020000,           // not used now (no area/zones with this flag set in 3.0.3)
	CONTESTED_AREA = 0x00040000,           // On PvP servers these areas are considered contested, even though the zone it is contained in is a Horde/Alliance territory.
	UNK6 = 0x00080000,           // Valgarde and Acherus: The Ebon Hold

	LOWLEVEL = 0x00100000,           // used for some starting areas with area_level <=15
	TOWN = 0x00200000,           // small towns with Inn
	UNK7 = 0x00400000,           // Warsong Hold, Acherus: The Ebon Hold, New Agamand Inn, Vengeance Landing Inn
	UNK8 = 0x00800000,           // Westguard Inn, Acherus: The Ebon Hold, Valgarde

	use_combat_world_state = 0x01000000,           // Wintergrasp and it's subzones
	INSIDE = 0x02000000,           // unknown
	OUTSIDE = 0x04000000,           // unknown
	CanHearthAndResurrectFromArea = 0x08000000,           // Wintergrasp and it's subzones

	CANNOT_FLY = 0x20000000,            // not allowed to fly, only used in Dalaran areas (zone 4395)
	use_parent_for_world_defense_visibility_check = 0x40000000,
};


DBC_DEF_BEGIN(DBC_AreaTable)

__DBC_REF_ID(DBC_Map,				MapID,			        2);
__DBC_REF_ID(DBC_AreaTable,			ParentArea,			    3);

__DBC_TVALUE(uint32,				AreaBits,				4);
__DBC_TVALUE(EAreaFlags,			Flags,					5);

//__DBC_TVALUE(iRefID, SoundProviderPrefferences, 6);
//__DBC_TVALUE(iRefID, Underwater_SoundProviderPrefferences, 7);
//__DBC_TVALUE(iRefID, SoundAmbience, 8);
//__DBC_TVALUE(iRefID, ZoneMusic, 9);
//__DBC_TVALUE(iRefID, ZoneIntroMusicTable, 10);

__DBC_TVALUE(int32,					ExplorationLevel,		11);
__DBC_LOCSTR(						AreaName,				12); // 12-28

__DBC_TVALUE(uint32,				FactionGroupMask,		29);

__DBC_REF_ID(DBC_LiquidType,		LiquidTypeIDWater,		30); // [0]: Water, [1]: Ocean, [2]: Magma, [3]: Slime
__DBC_REF_ID(DBC_LiquidType,        LiquidTypeIDOcean,      31);
__DBC_REF_ID(DBC_LiquidType,        LiquidTypeIDMagma,      32);
__DBC_REF_ID(DBC_LiquidType,        LiquidTypeIDSlime,      33);

__DBC_TVALUE(float,					MinElevation,			34); // -5000, -500, 1000
__DBC_TVALUE(float,					AmbientMultiplier,		35); // 0.0 - 1.0; Something to do with character ambient lightning
__DBC_REF_ID(DBC_Light,				LightId,				36);

DBC_DEF_END