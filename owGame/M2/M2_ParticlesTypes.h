#pragma once

#include __PACK_BEGIN

struct SM2_RibbonEmitter
{
	int32 ribbonId;					    // Always (as I have seen): -1.
	int32 boneIndex;					// A bone to attach to.
	vec3 position;						// And a position, relative to that bone.

	M2Array<uint16> textureIndices;		// into Textures
	M2Array<uint16> materialIndices;	// into materials

	M2Track<vec3> colorTrack;
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
};

//

struct SM2_Particle
{
	int32 particleId;                      // Always (as I have seen): -1.
	struct
	{
		uint32 AFFECTEDBYLIGHTING : 1;
		uint32 unk0 : 2;
		uint32 WORLDUP : 1;
		uint32 DONOTTRAIL : 1;
		uint32 INLIGHTING : 1;
		uint32 unk1 : 6;
		uint32 DONOTBILLBOARD : 1;
		uint32 : 19;
	} flags;

	glm::vec3 Position;						  // The position. Relative to the following bone.
	int16 bone;                               // The bone its attached to.

	uint16 texture;                           // And the m_DiffuseTextures that are used. 

	M2Array<char> geometry_model_filename;    // if given, this emitter spawns models
	M2Array<char> recursion_model_filename;   // if given, this emitter is an alias for the (maximum 4) emitters of the given model

	uint8 blendingType;                       // A blending type for the particle. See Below
	uint8 emitterType;                        // 1 - Plane (rectangle), 2 - Sphere, 3 - Spline, 4 - Bone
#ifdef WOW_BC
	uint16 particleColorIndex;                // This one is used for ParticleColor.dbc. See below.
#endif

	uint16 particleType;                       // Found below.
	uint16 headorTail;                         // 0 - Head, 1 - Tail, 2 - Both 

	int16 textureTileRotation;               // Rotation for the texture tile. (Values: -1,0,1) -- priorityPlane
	uint16 textureDimensions_rows;            // for tiled m_DiffuseTextures
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
#ifdef WOW_WOTLK
	float lifespanVary;                       // An individual particle's lifespan is added to by lifespanVary * random(-1, 1)
#endif

	M2Track<float> emissionRate;
#ifdef WOW_WOTLK
	float emissionRateVary;                   // This adds to the base emissionRate value the same way as lifespanVary. The random value is different every update.
#endif

	M2Track<float> emissionAreaLength;        // For plane generators, this is the width of the plane in the x-axis.
											  // For sphere generators, this is the minimum radius.

	M2Track<float> emissionAreaWidth;         // For plane generators, this is the width of the plane in the y-axis.
											  // For sphere generators, this is the maximum radius.

	M2Track<float> zSource;                   // When greater than 0, the initial velocity of the particle is (particle.position - C3Vector(0, 0, zSource)).Normalize()

#ifdef WOW_WOTLK
	M2TrackFake<vec3> colorTrack;             // Most likely they all have 3 timestamps for {start, middle, end}.
	M2TrackFake<short> alphaTrack;            // FIXME FIXED16
	M2TrackFake<vec2> scaleTrack;
	vec2 scaleVary;                           // A percentage amount to randomly vary the scale of each particle
	M2TrackFake<uint16> headCellTrack;        // Some kind of intensity values seen: 0,16,17,32 (if set to different it will have high intensity)
	M2TrackFake<uint16> tailCellTrack;
#else
	float midPoint;                           // middleTime; Middle point in lifespan (0 to 1).
	CImVector colorValues[3];                // start, middle, end
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

	float spin;                               // 0.0 for none, 1.0 to rotate the particle 360 degrees throughout its lifetime.

	M2Box tumble;
	vec3 WindVector;
	float WindTime;

	float followSpeed1;
	float followScale1;
	float followSpeed2;
	float followScale2;

	M2Array<vec3> splinePoints;     // Set only for spline praticle emitter. Contains array of points for spline
	M2Track<uint8> enabledIn;       // (boolean) Appears to be used sparely now, probably there's a flag that links particles to animation sets where they are enabled.
};

#include __PACK_END