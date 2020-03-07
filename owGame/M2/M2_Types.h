#pragma once

#include "DBC/DBC__Storage.h"

#include "M2_CommonTypes.h"

#include __PACK_BEGIN

struct SM2_Loop
{
	uint32 timestamp;
};

struct SM2_Sequence
{
	FOREIGN_KEY_ID(uint16, DBC_AnimationData, animID); // Animation id in AnimationData.dbc	
	uint16		variationIndex;			// Sub-animation id: Which number in a row of animations this one is.

	uint32_t start_timestamp;
	uint32_t end_timestamp;
	float		movespeed;				// This is the speed the character moves with in this animation.

	struct Flags
	{
		uint32 unk0 : 1;
		uint32 unk1 : 1;
		uint32 unk2 : 1;
		uint32 unk3 : 1;
		uint32 LowPrioritySeq : 1;
		uint32 DataInM2 : 1; // If set, the animation data is in the .m2 file. If not set, the animation data is in an .anim file.
		uint32 HasNext : 1; // (To find the animation data, the client skips these by following aliasNext until an animation without 0x40 is found.)
		uint32 IsBlended : 1; // (if either side of a transition has 0x80, lerp between end->start states, unless end==start by comparing bone values)
		uint32 : 24;
	} flags;
	int16		frequency;				// This is used to determine how often the animation is played. For all animations of the same type, this adds up to 0x7FFF (32767).
	uint16		unk0;
	M2Range		replay;					// May both be 0 to not repeat. Client will pick a random number of repetitions within bounds if given.
	uint32		blendTime;

	M2Bounds	bounds;
	int16		variationNext;			// id of the following animation of this AnimationID, points to an Index or is -1 if none.
	uint16		aliasNext;				// id in the list of animations. Used to find actual animation if this sequence is an alias (flags & 0x40)
};

struct SM2_Bone
{
	int32 key_bone_id;			// Back-reference to the key bone lookup table. -1 if this is no key bone.

	struct Flags
	{
		uint32 unk0 : 1;
		uint32 unk1 : 1;
		uint32 unk2 : 1;
		uint32 spherical_billboard : 1;
		uint32 cylindrical_billboard_lock_x : 1;
		uint32 cylindrical_billboard_lock_z : 1;
		uint32 cylindrical_billboard_lock_y : 1;
		uint32 unk3 : 1;
		uint32 unk4 : 1;
		uint32 transformed : 1;
		uint32 kinematic_bone : 1;			// MoP+: allow physics to influence this bone
		uint32 unk5 : 1;
		uint32 helmet_anim_scaled : 1;		// set blend_modificator to helmetAnimScalingRec.m_amount for this bone
		uint32 : 19;
	} flags;

	int16			parent_bone;            // Parent bone ID or -1 if there is none.
	uint16			submesh_id;				// Mesh part ID OR uDistToParent?

#if WOW_CLIENT_VERSION == WOW_BC_2_4_3
	union 
	{                         // only  BC?
		struct 
		{
			uint16_t uDistToFurthDesc;
			uint16_t uZRatioOfChain;
		} CompressData;               // No model has ever had this part of the union used.
		uint32_t boneNameCRC;         // these are for debugging only. their bone names match those in key bone lookup.
	};
#endif

	M2Track<vec3>       translation;
#if WOW_CLIENT_VERSION == WOW_CLASSIC_1_12_1
	M2Track<quat>	    rotation;
#elif WOW_CLIENT_VERSION >= WOW_BC_2_4_3
	M2Track<M2CompQuat>	rotation;
#endif
	M2Track<vec3>       scale;

	glm::vec3           pivot;					// The pivot point of that bone.

	inline bool IsAnimated() const
	{
		return translation.interpolation_type != Interpolations::INTERPOLATION_NONE ||
			rotation.interpolation_type       != Interpolations::INTERPOLATION_NONE ||
			scale.interpolation_type          != Interpolations::INTERPOLATION_NONE ||
			flags.spherical_billboard                                               ||
			flags.cylindrical_billboard_lock_x                                      ||
			flags.cylindrical_billboard_lock_y                                      ||
			flags.cylindrical_billboard_lock_z;
	}

	inline bool IsBillboard() const
	{
		return flags.spherical_billboard       ||
			flags.cylindrical_billboard_lock_x ||
			flags.cylindrical_billboard_lock_y ||
			flags.cylindrical_billboard_lock_z;
	}
};

struct SM2_Vertex
{
	glm::vec3	pos;				// 0-12
	uint8	    bone_weights[4];	// 12-16
	uint8	    bone_indices[4];	// 16-20
	glm::vec3	normal;				// 20-32
	glm::vec2	tex_coords[2];		// 32-40, 40-48		// two DiffuseTextures, depending on shader used
};

struct SM2_Vertex_znEngine
{
	glm::vec3 pos;
	struct
	{
		float weights[4];
	} bone_weights;
	struct
	{
		uint32 indexes[4];
	} bone_indices;
	glm::vec3 normal;
	glm::vec2 tex_coords[2];
};

#include __PACK_END

#include "M2_SkinTypes.h"

#include __PACK_BEGIN
struct SM2_Color
{
	M2Track<glm::vec3> color;     // vertex colors in rgb order
	M2Track<short> alpha;    // 0 - transparent, 0x7FFF - opaque
};

struct SM2_Texture
{
	enum Type : uint32
	{
		NONE = 0,			// Texture given in filename
		SKIN,				// Skin // Body + clothes
		OBJECT_SKIN,		// Object Skin // Item, Capes("Item\ObjectComponents\Cape\*.blp")
		WEAPON_BLADE,		// Weapon Blade // Used on several models but not used in the client as far as I see.Armor Reflect ?
		WEAPON_HANDLE,		// Weapon Handle
		ENVIRONMENT,		// (OBSOLETE)Environment(Please remove from source art)
		CHAR_HAIR,			// Character Hair
		CHAR_FACIAL_HAIR,	// (OBSOLETE)Character Facial Hair(Please remove from source art)
		SKIN_EXTRA,			// Skin Extra
		UI_SKIN,			// UI Skin // Used on inventory art M2s(1) : inventoryartgeometry.m2 and inventoryartgeometryold.m2
		TAUREN_MANE,		// (OBSOLETE)Tauren Mane(Please remove from source art) //Only used in quillboarpinata.m2.I can't even find something referencing that file. Oo Is it used?
		MONSTER_1,			// Monster Skin 1 //Skin for creatures or gameobjects #1
		MONSTER_2,			// Monster Skin 2 //Skin for creatures or gameobjects #2
		MONSTER_3,			// Monster Skin 3 //Skin for creatures or gameobjects #3
		ITEM_ICON,			// Item Icon // Used on inventory art M2s(2) : ui - button.m2 and forcedbackpackitem.m2(CSimpleModel_ReplaceIconTexture("texture"))

		COUNT
	} type;

	struct Flags
	{
		uint32 WRAPX : 1;
		uint32 WRAPY : 1;
		uint32 : 30;
	} flags;

	M2Array<char> filename; // for non-hardcoded textures (type != 0), this still points to a zero-sized std::string
};

struct SM2_TextureWeight
{
	M2Track<short> weight; // fixed 16
};

struct SM2_TextureTransform
{
	M2Track<vec3> translation;
	M2Track<quat> rotation;    // rotation center is texture center (0.5, 0.5)
	M2Track<vec3> scaling;
};

struct SM2_Material
{
	enum BlendModes : uint16
	{
		M2BLEND_OPAQUE = 0,
		M2BLEND_ALPHA_KEY,
		M2BLEND_ALPHA,
		M2BLEND_NO_ALPHA_ADD,
		M2BLEND_ADD,
		M2BLEND_MOD,
		M2BLEND_MOD2X,

		COUNT
	};

	struct Flags
	{
		uint16 UNLIT : 1;
		uint16 UNFOGGED : 1;
		uint16 TWOSIDED : 1;
		uint16 DEPTHTEST : 1;
		uint16 DEPTHWRITE : 1;
		uint16 : 11;
	} flags;
	BlendModes m_BlendMode;
};

struct SM2_Attachment
{
	uint32 id;                        // Referenced in the lookup-block below.
	uint16 bone;                      // attachment base
	uint16 unk0;
	vec3 position;                      // relative to bone; Often this value is the same as bone's pivot point 
	M2Track<uint8> animate_attached;  // whether or not the attached model is animated when this model is. only a bool is used. default is true.
};

struct SM2_Event
{
	char identifier[4];  // mostly a 3 character name prefixed with '$'.
	uint32 data;        // This data is passed when the event is fired. 
	uint32 bone;        // Somewhere it has to be attached.
	vec3 position;    // Relative to that bone of course, animated. Pivot without animating.
	M2TrackBase enabled;  // This is a timestamp-only animation block. It is built up the same as a normal AnimationBlocks, but is missing values, as every timestamp is an implicit "fire now".
};

struct SM2_Light
{
	enum Type : uint16
	{
		Directional = 0,
		Point
	} type;
	int16			bone;                       // -1 if not attached to a bone
	vec3			position;					// relative to bone, if given

	M2Track<vec3>	ambient_color;
	M2Track<float>	ambient_intensity;			// defaults to 1.0

	M2Track<vec3>	diffuse_color;
	M2Track<float>	diffuse_intensity;			// defaults to 1.0

	M2Track<float>	attenuation_start;
	M2Track<float>	attenuation_end;

	M2Track<uint8>	visibility;					// enabled?
};

struct SM2_Camera // TODO Spline keys
{
	uint32 type; // 0: portrait, 1: characterinfo; -1: else (flyby etc.); referenced backwards in the lookup table.

	float fov;
	float far_clip;
	float near_clip;

	M2Track<vec3> positions; // How the camera's position moves. Should be 3*3 floats.
	vec3 position_base;

	M2Track<vec3> target_position; // How the target moves. Should be 3*3 floats.
	vec3 target_position_base;

	M2Track<float> roll; // The camera can have some roll-effect. Its 0 to 2*Pi. 
};
#include __PACK_END

#include "M2_Animated.h"
#include "M2_ParticlesTypes.h"
