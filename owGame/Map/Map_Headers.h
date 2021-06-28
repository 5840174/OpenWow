#pragma once

// MapTile Header
struct SMapTile_MHDR
{
	struct Flags
	{
		uint32 HasMFBO : 1;
#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
		uint32 IsNortrend : 2;           // is set for some northrend ones.
#endif
	} flags;

	uint32 MCIN;
	uint32 MTEX;
	uint32 MMDX;
	uint32 MMID;
	uint32 MWMO;
	uint32 MWID;
	uint32 MDDF;
	uint32 MODF;
	uint32 MFBO;                     // this is only set if flags & mhdr_MFBO.
	uint32 MH20;
	uint32 MTXF;

	uint8 unk1[16];
};

// MapTile Texture info
struct SMapTile_MTEX
{
	std::string                 textureName;
#if 0
	std::shared_ptr<IImage>    diffuseTexture;
	std::shared_ptr<IImage>    specularTexture;
#else
	std::shared_ptr<ITexture> diffuseTexture;
	std::shared_ptr<ITexture> specularTexture;
#endif
};

// MapTile Chunk info
struct SMapTile_MCIN
{
	uint32_t offset;               // absolute offset.
	uint32_t size;                 // the size of the MCNK chunk, this is refering to.
	uint32_t flags;                // always 0. only set in the client., FLAG_LOADED = 1
	uint32_t asyncId;
};

// MapTile M2 model placement info
struct SMapTile_MDDF
{
	uint32    nameIndex;
	uint32    uniqueId;
	glm::vec3 position;
	glm::vec3 rotation;
	uint16    scale;
	struct
	{
		uint16 Flag_Biodome : 1;
		uint16 Flag_Shrubbery : 1;
		uint16 : 14;
	} flags;
};

// MapTile WMO model placement info
struct SMapTile_MODF
{
	uint32 nameIndex;		// Ref to MWID
	uint32 uniqueId;		// this ID should be unique for all ADTs currently loaded. Best, they are unique for the whole map.
	glm::vec3 position;
	glm::vec3 rotation;
	CAaBox boundingBox;		// position plus the transformed wmo bounding box. used for defining if they are rendered as well as collision.
	struct Flags
	{
		uint16 Destroyable : 1;
		uint16 : 15;
	} flags;
	uint16 doodadSetIndex;	// which WMO doodad set is used.
	uint16 nameSet;			// which WMO name set is used. Used for renaming goldshire inn to northshire inn while using the same model.
	uint16 scale;			// Legion+
};



// MapChunk Header
struct SMapChunk_MCNK
{
	struct Flags
	{
		uint32 has_mcsh : 1;
		uint32 impass : 1;
		uint32 lq_river : 1;
		uint32 lq_ocean : 1;
		uint32 lq_magma : 1;
		uint32 lq_slime : 1;
		uint32 has_mccv : 1;
		uint32 unk1 : 1;
		uint32 unk2 : 7;
		uint32 do_not_fix_alpha_map : 1; //"fix" alpha maps in MCAL and MCSH(4 bit alpha maps are 63 * 63 instead of 64 * 64).
		uint32 : 16;
	} flags;

	uint32 indexX;
	uint32 indexY;

	uint32 nLayers;
	uint32 nDoodadRefs;

	uint32 ofsHeight;
	uint32 ofsNormal;
	uint32 ofsLayer;
	uint32 ofsRefs;

	uint32 ofsAlpha;
	uint32 sizeAlpha;

	uint32 ofsShadow;
	uint32 sizeShadow;

	uint32 areaid;
	uint32 nMapObjRefs;

	uint16 holes;

	uint8 unk0[22];

	uint32 nEffectDoodad;

	uint32 ofsSndEmitters;
	uint32 nSndEmitters;

	uint32 ofsLiquid;
	uint32 sizeLiquid;

	float zpos;
	float xpos;
	float ypos;

	uint32_t ofsMCCV;                             // only with flags.has_mccv, had uint32_t textureId; in ObscuR's structure.

	uint8 unk1[8];
};

// MapChunk Texture info
struct SMapChunk_MCLY
{
	uint32 textureIndex;

	struct Flags
	{
		uint32 animation_rotation : 3;        // each tick is 45
		uint32 animation_speed : 3;
		uint32 animation_enabled : 1;
		uint32 overbright : 1;                // This will make the texture way brighter. Used for lava to make it "glow".
		uint32 use_alpha_map : 1;             // set for every layer after the first
		uint32 alpha_map_compressed : 1;      // see MCAL chunk description
		uint32 use_cube_map_reflection : 1;   // This makes the layer behave like its a reflection of the skybox. See below
		uint32 : 21;
	} flags;

	uint32 offsetInMCAL;
	FOREIGN_KEY_ID(uint32, DBC_GroundEffectTexture, effectId);
};