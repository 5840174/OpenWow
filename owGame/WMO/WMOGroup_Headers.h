#pragma once

#include "WMO_Shared.h"

#ifdef USE_WMO_MODELS

#pragma pack(push,1)

// WMOGroup Header
struct SWMOGroup_MOGP
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

// WMOGroup Material info
struct SWMOGroup_MOPY
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

// WMOGroup Batch definition
struct SWMOGroup_MOBA
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

// WMOGroup Collision definition
struct SWMOGroup_MOBN
{
	enum Flags
	{
		Flag_XAxis    = 0x00,
		Flag_ZAxis    = 0x01,
		Flag_mYAxis   = 0x02,
		Flag_AxisMask = 0x03,

		Flag_Leaf     = 0x04,

		Flag_NoChild  = 0xFFFF
	};

	uint16  flags;        // See above enum. 4: leaf, 0 for YZ-plane, 1 for XZ-plane, 2 for XY-plane
	int16   negChild;     // index of bsp child node (right in this array)
	int16   posChild;
	uint16  nFaces;       // num of triangle faces in MOBR
	uint32  faceStart;    // index of the first triangle index(in MOBR)
	float   planeDist;
};

// WMOGroup Liquid definition
struct SWMOGroup_MLIQ
{
	uint32    X, Y;       // number of vertices (x, y)
	uint32    A, B;       // number of tiles (ntiles = nverts-1)
	glm::vec3 pos;        // base coordinates for X and Y
	uint16    materialID; // material ID (index into MOMT)
};

#pragma pack(pop)

#endif