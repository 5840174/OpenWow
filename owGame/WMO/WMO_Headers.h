#pragma once

#include "DBC/DBC__Storage.h"

#include __PACK_BEGIN

struct SWMO_HeaderDef
{
	uint32 nTextures;
	uint32 nGroups;
	uint32 nPortals;
	uint32 nLights;
	uint32 nDoodadNames;
	uint32 nDoodadDefs;
	uint32 nDoodadSets;
	uint32 ambColor; // CArgb
	FOREIGN_KEY(uint32, DBC_WMOAreaTable, WMOID, wmoID);
	CAaBox bounding_box;
	struct Flags
	{
		uint16 attenuate_vertices_based_on_distance_to_portal : 1;	// delete vertexes
		uint16 skip_base_color : 1;									// do not add base (ambient) color (of MOHD) to MOCVs. apparently does more, e.g. ~!required!~ for multiple MOCVs
		uint16 use_liquid_type_dbc_id : 1;							// use real liquid type ID from DBCs instead of local one. See MLIQ for further reference.
		uint16 lighten_interiors : 1;								// makes iterior groups much brighter, effects MOCV rendering. Used e.g.in Stormwind for having shiny bright interiors,
		uint16 : 12;
	} flags;
	uint16 unk0;										  // ≥ Legion (21108) includes base lod (→ numLod = 3 means '.wmo', 

	//--

	/*vec4 getAmbColor() const
	{
		return vec4
		(
			static_cast<float>(ambColor.r) / 255.0f,
			static_cast<float>(ambColor.g) / 255.0f,
			static_cast<float>(ambColor.b) / 255.0f,
			static_cast<float>(ambColor.a) / 255.0f
		);
	}*/
};



struct SWMO_MaterialDef
{
	struct
	{
		uint32 DisableLighting : 1;            // disable lighting logic in shader (but can still use vertex colors)
		uint32 DisableFog : 1;                 // disable fog shading (rarely used)
		uint32 IsTwoSided : 1;                 // two-sided
		uint32 F_EXTLIGHT : 1;                 // darkened, the intern face of windows are flagged 0x08
		uint32 F_SIDN : 1;                     // (bright at night, unshaded) (used on windows and lamps in Stormwind, for example) (see emissive color)
		uint32 F_WINDOW : 1;                   // lighting related (flag checked in CMapObj::UpdateSceneMaterials)
		uint32 TextureClampS : 1;              // tex clamp S (force this material's m_DiffuseTextures to use clamp s addressing)
		uint32 TextureClampT : 1;              // tex clamp T (force this material's m_DiffuseTextures to use clamp t addressing)
		uint32 : 24;
	} flags;

	uint32 shader;								// Index into CMapObj::s_wmoShaderMetaData. See below (shader types).
	uint32 blendMode;

	uint32 diffuseNameIndex;					// offset into MOTX

	CBgra emissive_color;						// emissive color; see below (emissive color)
	CBgra sidn_emissive_color;					// set at runtime; gets sidn-manipulated emissive color; see below (emissive color)

	uint32 envNameIndex;					    // offset into MOTX

	uint32 diffColor;

	FOREIGN_KEY_ID(uint32, DBC_TerrainType, ground_type);

	uint32 texture_2;
	uint32 color_2;  // liquid diffuse
	uint32 flags_2;

	uint32 unk0[4];
};

struct SWMO_PortalDef
{
	uint16 startVertex;
	uint16 count;
	C4Plane plane;
};

struct SWMO_PortalReferencesDef
{
	uint16 portalIndex;  // into MOPR
	uint16 groupIndex;   // the other one
	int16 side;          // positive or negative.
	uint16 unk0;
};

struct SWMO_VisibleBlockListDef
{
	uint16 firstVertex;
	uint16 count;
};

struct SWMO_LightDef
{
	enum : uint8
	{
		OMNI_LGT = 0,
		SPOT_LGT = 1,
		DIRECT_LGT = 2,
		AMBIENT_LGT = 3,
	} type ;
	uint8 useAtten;
	uint8 unk0[2];
	uint32 color;
	glm::vec3 pos;
	float intensity;
	float attenStart;
	float attenEnd;
	float unk1[4];
};

// Doodads

struct SWMO_Doodad_SetInfo
{
	SWMO_Doodad_SetInfo() :
		name("none"),
		start(UINT32_MAX),
		size(UINT32_MAX)
	{}

	char name[20];		// Set name
	uint32 start;		// index of first doodad instance in this set
	uint32 size;		// number of doodad instances in this set
	uint32 unk0;		// always 0

	inline bool InSet(uint32 _index) const
	{
		return ((_index >= start) && (_index < (start + size)));
	}
};

struct SWMO_Doodad_PlacementInfo
{
	struct Flags
	{
		uint32 nameIndex : 24;          // reference offset into MODN
		uint32 flag_AcceptProjTex : 1;
		uint32 : 7;                     // unused as of 7.0.1.20994
	} flags;

	glm::vec3 position;		    // (X,Z,-Y)
	C4ImQuaternion orientation;	// (X, Y, Z, W)
	float scale;				// scale factor
	CBgra color;				// (B,G,R,A) diffuse lighting color, used in place of global diffuse from DBCs

	inline glm::vec4 getColor() const
	{
		return glm::vec4(
			static_cast<float>(color.r) / 255.0f,
			static_cast<float>(color.g) / 255.0f,
			static_cast<float>(color.b) / 255.0f,
			static_cast<float>(color.a) / 255.0f
		);
	}
};

struct SWMO_FogDef
{
	struct Flags
	{
		uint32 flag_infinite_radius : 1; // F_IEBLEND: Ignore radius in CWorldView::QueryCameraFog
		uint32 : 31;
	} flags;

	glm::vec3 position;
	float smallerRadius; // Smaller radius
	float largerRadius;  // Larger radius

	struct Fog
	{
		float end;			// Fog end
		float startScalar;	// Fog start multiplier (0..1)
		CBgra color;	// The back buffer is also cleared to this colour 
	} fog;

	struct UnderwaterFog
	{
		float end;
		float startScalar;
		CBgra color;
	} underwater_fog;
};

#include __PACK_END