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

#if WOW_CLIENT_VERSION <= WOW_BC_2_4_3
	uint32_t start_timestamp;
	uint32_t end_timestamp;
#else
	uint32_t duration;
#endif
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

#if WOW_CLIENT_VERSION >= WOW_BC_2_4_3
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

	M2Track<glm::vec3>       translation;
#if WOW_CLIENT_VERSION == WOW_CLASSIC_1_12_1
	M2Track<glm::quat>	    rotation;
#elif WOW_CLIENT_VERSION >= WOW_BC_2_4_3
	M2Track<M2CompQuat>	rotation;
#endif
	M2Track<glm::vec3>       scale;

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
	M2Track<glm::vec3> translation;
	M2Track<glm::quat> rotation;    // rotation center is texture center (0.5, 0.5)
	M2Track<glm::vec3> scaling;
};



struct SM2_Material
{
	struct Flags
	{
		uint16 UNLIT : 1;
		uint16 UNFOGGED : 1;
		uint16 TWOSIDED : 1;
		uint16 DEPTHTEST : 1;
		uint16 DEPTHWRITE : 1;
		uint16 unk0x20 : 1;
		uint16 unk0x40 : 1;
		uint16 unk0x80 : 1;
		uint16 unk0x100 : 1;
		uint16 unk0x200 : 1;
		uint16 unk0x400 : 1;
		uint16 unk0x800 : 1;
		uint16 unk0x1000 : 1;
		uint16 unk0x2000 : 1;
		uint16 unk0x4000 : 1;
		uint16 unk0x8000 : 1;
	} flags;

	enum BlendModes : uint16
	{
		M2BLEND_OPAQUE = 0,
		M2BLEND_ALPHA_KEY,
		M2BLEND_ALPHA,
		M2BLEND_NO_ALPHA_ADD,
		M2BLEND_ADD,
		M2BLEND_MOD,
		M2BLEND_MOD2X
	} BlendMode;
};



struct SM2_Attachment
{
	uint32 id;                        // Referenced in the lookup-block below.
	uint16 bone;                      // attachment base
	uint16 unk0;
	glm::vec3 position;               // relative to bone; Often this value is the same as bone's pivot point 
	M2Track<uint8> animate_attached;  // whether or not the attached model is animated when this model is. only a bool is used. default is true.
};



struct SM2_Event
{
	char identifier[4];    // mostly a 3 character name prefixed with '$'.
	uint32 data;           // This data is passed when the event is fired. 
	uint32 bone;           // Somewhere it has to be attached.
	glm::vec3 position;    // Relative to that bone of course, animated. Pivot without animating.
	M2TrackBase enabled;   // This is a timestamp-only animation block. It is built up the same as a normal AnimationBlocks, but is missing values, as every timestamp is an implicit "fire now".
};



struct SM2_Light
{
	enum Type : uint16
	{
		Directional = 0,
		Point
	} type;
	int16			    bone;                       // -1 if not attached to a bone
	glm::vec3		    position;					// relative to bone, if given

	M2Track<glm::vec3>	ambient_color;
	M2Track<float>	    ambient_intensity;			// defaults to 1.0

	M2Track<glm::vec3>	diffuse_color;
	M2Track<float>	    diffuse_intensity;			// defaults to 1.0

	M2Track<float>	    attenuation_start;
	M2Track<float>	    attenuation_end;

	M2Track<uint8>	    visibility;					// enabled?
};



struct SM2_Camera // TODO Spline keys
{
	uint32              type; // 0: portrait, 1: characterinfo; -1: else (flyby etc.); referenced backwards in the lookup table.

	float               fov;
	float               far_clip;
	float               near_clip;

	M2Track<glm::vec3>  positions; // How the camera's position moves. Should be 3*3 floats.
	glm::vec3           position_base;

	M2Track<glm::vec3>  target_position; // How the target moves. Should be 3*3 floats.
	glm::vec3           target_position_base;

	M2Track<float>      roll; // The camera can have some roll-effect. Its 0 to 2*Pi. 
};



struct SM2_RibbonEmitter
{
	int32 ribbonId;					    // Always (as I have seen): -1.
	int32 boneIndex;					// A bone to attach to.
	glm::vec3 position;						// And a position, relative to that bone.

	M2Array<uint16> textureIndices;		// into Textures
	M2Array<uint16> materialIndices;	// into materials

	M2Track<glm::vec3> colorTrack;
	M2Track<short> alphaTrack;          // And an alpha value in a short, where: 0 - transparent, 0x7FFF - opaque.
	M2Track<float> heightAboveTrack;
	M2Track<float> heightBelowTrack;	// do not set to same!

	float edgesPerSecond;				// this defines how smooth the ribbon is. A low value may produce a lot of edges.
	float edgeLifetime;					// the length aka Lifespan. in seconds
	float gravity;                      // use arcsin(val) to get the emission angle in degree

	uint16 textureRows;					// tiles in texture
	uint16 textureCols;

	M2Track<uint16> texSlotTrack;
	M2Track<uint8> visibilityTrack;

#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
	int16_t priorityPlane;
	uint16_t padding;
#endif
};

struct SM2_Particle
{
	int32 particleId;                      // Always (as I have seen): -1.
	struct Flags
	{
		uint32 AFFECTEDBYLIGHTING : 1;     // Particles are affected by lighting;
		uint32 unk0x2 : 1;
		uint32 unk0x4 : 1;
		uint32 WORLDUP : 1;                // Particles travel "up" in world space, rather than model.
		uint32 DONOTTRAIL : 1;             // Do not Trail
		uint32 INLIGHTING : 1;             // Unlightning
		uint32 unk0x40 : 1;                // Use Burst Multiplier
		uint32 unk0x80 : 1;                // Particles in Model Space
		uint32 unk0x100 : 1;
		uint32 unk0x200 : 1;               // spawn position randomized in some way?
		uint32 unk0x400 : 1;               // STYLE: Pinned Particles, their quad enlarges from their creation position to where they expand.
		uint32 unk0x800 : 1;
		uint32 DONOTBILLBOARD : 1;         // XYQuad Particles. They align to XY axis facing Z axis direction.
		uint32 unk0x2000 : 1;              // clamp to ground; call CParticleEmitter2::ProjectParticle
		uint32 unk0x4000 : 1;
		uint32 unk0x8000 : 1;
		uint32 unk0x10000 : 1;             // ChooseRandomTexture
		uint32 unk0x20000 : 1;             // STYLE: "Outward" particles, most emitters have this and their particles move away from the origin, when they don't the particles start at origin+(speed*life) and move towards the origin.
		uint32 unk0x40000 : 1;             // STYLE: unknown. In a large proportion of particles this seems to be simply the opposite of the above flag, but in some (e.g. voidgod.m2 or wingedlionmount.m2) both flags are true.
		uint32 : 13;
	} flags;

	glm::vec3 Position;						// The position. Relative to the following bone.
	int16 bone;                             // The bone its attached to.

	uint16 texture;                           // And the DiffuseTextures that are used. 

	M2Array<char> geometry_model_filename;    // if given, this emitter spawns models
	M2Array<char> recursion_model_filename;   // if given, this emitter is an alias for the (maximum 4) emitters of the given model

#if (WOW_CLIENT_VERSION >= WOW_BC_2_4_3)
	uint8 blendingType;                       // A blending type for the particle. See Below
	uint8 emitterType;                        // 1 - Plane (rectangle), 2 - Sphere, 3 - Spline, 4 - Bone
	uint16 particleColorIndex;                // This one is used for ParticleColor.dbc. See below.
#else
	uint16 blendingType;                      // A blending type for the particle. See Below
	uint16 emitterType;                       // 1 - Plane (rectangle), 2 - Sphere, 3 - Spline, 4 - Bone
#endif

	uint8 particleType;                      // Found below.
	uint8 headorTail;                        // 0 - Head, 1 - Tail, 2 - Both 

	int16 textureTileRotation;                // Rotation for the texture tile. (Values: -1,0,1) -- priorityPlane
	uint16 textureDimensions_rows;            // for tiled DiffuseTextures
	uint16 textureDimensions_columns;

	M2Track<float> emissionSpeed;             // Base velocity at which particles are emitted.
	M2Track<float> speedVariation;            // Random variation in particle emission speed. (range: 0 to 1)
	
	M2Track<float> verticalRange;             // Drifting away vertically. (range: 0 to pi) For plane generators, this is the maximum polar angle of the initial velocity; 
											  // 0 makes the velocity straight up (+z). For sphere generators, this is the maximum elevation of the initial position; 
											  // 0 makes the initial position entirely in the x-y plane (z=0).
	
	M2Track<float> horizontalRange;           // They can do it horizontally too! (range: 0 to 2*pi) For plane generators, this is the maximum azimuth angle of the initial velocity; 
											  // 0 makes the velocity have no sideways (y-axis) component. 
											  // For sphere generators, this is the maximum azimuth angle of the initial position.

	M2Track<float> gravity;                   // Not necessarily a float; see below.

	M2Track<float> lifespan;
#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
	float lifespanVary;                       // An individual particle's lifespan is added to by lifespanVary * random(-1, 1)
#endif

	M2Track<float> emissionRate;
#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
	float emissionRateVary;                   // This adds to the base emissionRate value the same way as lifespanVary. The random value is different every update.
#endif

	M2Track<float> emissionAreaLength;        // For plane generators, this is the width of the plane in the x-axis.
											  // For sphere generators, this is the minimum radius.

	M2Track<float> emissionAreaWidth;         // For plane generators, this is the width of the plane in the y-axis.
											  // For sphere generators, this is the maximum radius.

	M2Track<float> zSource;                   // When greater than 0, the initial velocity of the particle is (particle.position - C3Vector(0, 0, zSource)).Normalize()

#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
	M2TrackFake<glm::vec3> colorTrack;        // Most likely they all have 3 timestamps for {start, middle, end}.
	M2TrackFake<short> alphaTrack;            // FIXME FIXED16
	M2TrackFake<glm::vec2> scaleTrack;
	glm::vec2 scaleVary;                      // A percentage amount to randomly vary the scale of each particle
	M2TrackFake<uint16> headCellTrack;        // Some kind of intensity values seen: 0,16,17,32 (if set to different it will have high intensity)
	M2TrackFake<uint16> tailCellTrack;
#else
	float midPoint;                           // middleTime; Middle point in lifespan (0 to 1).
	CBgra colorValues[3];                     // start, middle, end
	float scaleValues[3];
	uint16 lifespanUVAnim[3];
	uint16 decayUVAnim[3];
	int16 tailUVAnim[2];                      // start, end
	int16 tailDecayUVAnim[2];
#endif

	float tailLength;
	float twinkleSpeed;                       // twinkleFPS; has something to do with the spread
	float twinklePercent;                     // same mechanic as MDL twinkleOnOff but non-binary in 0.11.0
	CRange twinkleScale;                     // min, max
	float burstMultiplier;                    // ivelScale; requires (flags & 0x40)
	float drag;                               // For a non-zero values, instead of travelling linearly the particles seem to slow down sooner. Speed is multiplied by exp( -drag * t ).

#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
	float baseSpin;                           // Initial rotation of the particle quad
	float baseSpinVary;
	float spin;                               // Rotation of the particle quad per second
	float spinVary;
#else
	float spin;                               // 0.0 for none, 1.0 to rotate the particle 360 degrees throughout its lifetime.
#endif

	M2Box tumble;
	glm::vec3 WindVector;
	float WindTime;

	float followSpeed1;
	float followScale1;
	float followSpeed2;
	float followScale2;

	M2Array<glm::vec3> splinePoints;     // Set only for spline praticle emitter. Contains array of points for spline
	M2Track<uint8> enabledIn;       // (boolean) Appears to be used sparely now, probably there's a flag that links particles to animation sets where they are enabled.
};

#include __PACK_END

#include "M2_Animated.h"
