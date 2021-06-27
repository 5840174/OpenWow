#pragma once

#ifdef USE_WMO_MODELS

#pragma pack(push,1)

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

struct SWMO_GroupInfoDef
{
	SWMOGroup_Flags	flags;
	CAaBox			bounding_box;
	int32			nameoffset;		// name in MOGN chunk (-1 for no name)
};

struct SWMO_Group_HeaderDef
{
	uint32          groupName;				// offset into MOGN
	uint32          descriptiveGroupName;	// offset into MOGN
	SWMOGroup_Flags flags;
	CAaBox          boundingBox;

	uint16          portalStart; // Index into the MOPR chunk
	uint16          portalCount; // Number of items used from the MOPR chunk

	// Batches
	uint16 transBatchCount;
	uint16 intBatchCount;
	uint16 extBatchCount;
	uint16 unk0;

	uint8           fogIds[4]; // Up to four indices into the WMO fog list
	uint32          liquidType; // LiquidType related, see below in the MLIQ chunk.

	FOREIGN_KEY(uint32, DBC_WMOAreaTable, WMOGroupID, wmoGroupId);

	uint32 unk1;
	uint32 unk2;
};

struct SWMO_Group_MaterialDef
{
	struct Flags
	{
		uint8 UNK_0x01     : 1;
		uint8 NOCAMCOLLIDE : 1;
		uint8 DETAIL       : 1;
		uint8 COLLISION    : 1; // Turns off rendering of water ripple effects. May also do more. Should be used for ghost material triangles.
		uint8 HINT         : 1;
		uint8 RENDER       : 1;
		uint8 UNK_0x40     : 1;
		uint8 COLLIDE_HIT  : 1;
	} flags;

	uint8 materialId; // 0xff for collision

	bool isTransFace()  { return flags.UNK_0x01 && (flags.DETAIL || flags.RENDER); }
	bool isColor()      { return !flags.COLLISION; }
	bool isRenderFace() { return flags.RENDER && !flags.DETAIL; }
	bool isCollidable() { return flags.COLLISION || isRenderFace(); }
};

struct SWMO_Group_BatchDef
{
	int16  bx, by, bz;                      // a bounding box for culling, see "unknown_box" below
	int16  tx, ty, tz;

	uint32 indexStart;
	uint16 indexCount;

	uint16 vertexStart;
	uint16 vertexEnd;

	uint8  flags;
	uint8  material_id; // index in MOMT

	inline uint16 getVerticesCount() const { return vertexEnd - vertexStart; }
};

struct SWMO_Group_MOBNDef
{
	enum Flags
	{
		Flag_XAxis = 0x0,
		Flag_YAxis = 0x1,
		Flag_ZAxis = 0x2,
		Flag_AxisMask = 0x3,
		Flag_Leaf = 0x4,
		Flag_NoChild = 0xFFFF,
	};

	uint16_t flags;        // See above enum. 4: leaf, 0 for YZ-plane, 1 for XZ-plane, 2 for XY-plane
	int16_t negChild;      // index of bsp child node (right in this array)
	int16_t posChild;
	uint16_t nFaces;       // num of triangle faces in MOBR
	uint32_t faceStart;    // index of the first triangle index(in MOBR)
	float planeDist;
};

struct SWMO_Group_MLIQDef
{
	uint32    X, Y;       // number of vertices (x, y)
	uint32    A, B;       // number of tiles (ntiles = nverts-1)
	glm::vec3 pos;        // base coordinates for X and Y
	uint16    materialID; // material ID (index into MOMT)
};

#pragma pack(pop)

#endif