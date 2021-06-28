#pragma once

struct SWMOGroup_Flags
{
	uint32 HAS_COLLISION : 1;					//Has MOBN and MOBR chunk.
	uint32 UNK_0x2 : 1;							// UNUSED
	uint32 HAS_MOCV : 1;				        //Has vertex colors(MOCV chunk).
	uint32 IS_OUTDOOR : 1;                      //If camera is AABB present in a group with this flag, and not present in any group with SMOGroup::INTERIOR, render all exteriors

	uint32 UNK_0x10 : 1;
	uint32 UNK_0x20 : 1;
	uint32 DO_NOT_USE_LIGHTING_DIFFUSE : 1;		// Do not use local diffuse lightning. Applicable for both doodads from this wmo group(color from MODD) and water(CWorldView::GatherMapObjDefGroupLiquids).
	uint32 IS_UNREACHABLE : 1;

	uint32 UNK_0x100 : 1;                       // Show exterior sky in interior WMO group (Used for interiors of city in stratholme_past.wmo)
	uint32 HAS_LIGHTS : 1;						// Has Lights (MOLR chunk)
	uint32 UNK_0x400 : 1;
	uint32 HAS_DOODADS : 1;						// Has doodads (MODR chunk)

	uint32 HAS_WATER : 1;						// Has water(MLIQ chunk)
	uint32 IS_INDOOR : 1;						// Indoor
	uint32 UNK_0x4000 : 1;
	uint32 UNK_0x8000 : 1;

	uint32 IS_ALWAYS_DRAW : 1;					// Clear 0x8 after CMapObjGroup::Create() in MOGP and MOGI
	uint32 HAS_MORI_AND_MORB : 1;				// Has MORI and MORB chunks.
	uint32 HAS_SKYBOX : 1;						// automatically unset if MOSB not present.
	uint32 IS_NOT_WATER_BUT_OCEAN : 1;			// LiquidType related, see below in the MLIQ chunk.

	uint32 UNK_0x100000 : 1;
	uint32 IS_MOUNT_ALLOWED : 1;
	uint32 UNK_0x400000 : 1;
	uint32 UNK_0x800000 : 1;

	uint32 HAS_2_MOCV : 1;						// Has two MOCV chunks: Just add two or don't set 0x4 to only use cverts2.
	uint32 HAS_2_MOTV : 1;						// Has two MOTV chunks: Just add two.
	uint32 IS_ANTIPORTAL : 1;					// Just call CMapObjGroup::CreateOccluders() independent of groupname being "antiportal".requires intBatchCount == 0, extBatchCount == 0, UNREACHABLE
	uint32 UNK_0x8000000 : 1;					// requires intBatchCount == 0, extBatchCount == 0, UNREACHABLE.

	uint32 UNK_0x10000000 : 1;
	uint32 IS_EXTERIOR_CULL : 1;
	uint32 HAS_3_MOTV : 1;						// Has three MOTV chunks, eg. for MOMT with shader 18.
	uint32 UNK_0x80000000 : 1;
};
