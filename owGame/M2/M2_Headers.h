#pragma once

#include "M2_Types.h"

#include __PACK_BEGIN

struct SM2_Header
{
    char							magic[4];						// MD20 Magic
    uint32							version;
    M2Array<char>					name;							// should be globally unique, used to reload by name in internal clients

    struct Flags
    {
        uint32 flag_tilt_x : 1;
        uint32 flag_tilt_y : 1;
		uint32 unk0 : 1;
		uint32 flag_use_texture_combiner_combos : 1;
        uint32 : 28;
    }								global_flags;
    M2Array<SM2_Loop>				global_loops;					// Timestamps used in global looping animations.


    // Sequences
    M2Array<SM2_Sequence>			sequences;						// Information about the animations in the model.
    M2Array<int16>					sequencesLookup;				// Mapping of sequence IDs to the entries in the Animation sequences block.
    M2Array<int16>					playable_animation_lookup;


    // Bones
    M2Array<SM2_Bone>				bones;                           // MAX_BONES = 0x100
    M2Array<uint16>					gameBonesLookup;                 // Lookup table for game bones

    // Vertices
    M2Array<SM2_Vertex>				vertices;

	// Skin
    M2Array<SM2_SkinSection> skin_profiles;

    // Materials
    M2Array<SM2_Color>				colors;							// Color and alpha animations definitions.
    M2Array<SM2_Texture>			textures;
    M2Array<SM2_TextureWeight>		textureWeights;
    M2Array<void>                   unknown;
	M2Array<SM2_TextureTransform>	textureTransforms;

    M2Array<uint16>					replacable_texture_lookup;		// ???
    M2Array<SM2_Material>			materials;						// Blending modes / render flags.
	M2Array<uint16>					bonesLookup;
    M2Array<uint16>					textureLookup;
	M2Array<uint16>					textureUnitLookup;				// -1 env mapping, 0, 1 - indexes
    M2Array<uint16>					textureWeightsLookup;
    M2Array<uint16>					textureTransformsLookup;


    // Bounding box
    CAaBox							bounding_box;					// min/max( [1].z, 2.0277779f ) - 0.16f seems to be the maximum camera height
    float							bounding_sphere_radius;			// detail doodad draw dist = clamp (bounding_sphere_radius * detailDoodadDensityFade * detailDoodadDist, …)

	// Collision stuff
    CAaBox							collision_box;
    float							collision_sphere_radius;
    M2Array<uint16>					collisionTriangles;
    M2Array<vec3>					collisionVertices;
    M2Array<vec3>					collisionNormals;

    // Attachments
    M2Array<SM2_Attachment>			attachments;
    M2Array<uint16>					attachmentLookup;

    // Events
    M2Array<SM2_Event>				events;

    // Light
    M2Array<SM2_Light>				lights;

    // Camara
    M2Array<SM2_Camera>				cameras;
    M2Array<uint16>					camerasLookup;

    // Emitters and particles
    M2Array<SM2_RibbonEmitter>		ribbon_emitters;
    M2Array<SM2_Particle>			particle_emitters;
};

#include __PACK_END