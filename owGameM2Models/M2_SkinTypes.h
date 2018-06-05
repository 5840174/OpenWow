#pragma once

#include __PACK_BEGIN

struct SM2_SkinBones
{
	uint8 index[4];
};

struct SM2_SkinSection
{
	uint16 meshPartID;
	uint16 level;               // (level << 16) is added (|ed) to startTriangle and alike to avoid having to increase those fields to uint32s.

	uint16 vertexStart;         // Starting vertex number.
	uint16 vertexCount;         // Number of vertices.

	uint16 indexStart;          // Starting triangle index (that's 3* the number of triangles drawn so far).
	uint16 indexCount;          // Number of triangle indices.

	uint16 boneCount;           // Number of elements in the bone lookup table.
	uint16 bonesStartIndex;     // Starting index in the bone lookup table.
	uint16 boneInfluences;      // <= 4	 // from <=BC documentation: Highest number of bones needed at one time in this Submesh --Tinyn (wowdev.org) 
								// In 2.x this is the amount of of bones up the parent-chain affecting the submesh --NaK
	uint16 centerBoneIndex;
	vec3   centerPosition;      // Average position of all the vertices in the sub mesh.
};

struct SM2_SkinBatch
{
	struct Flags
	{
		uint8 Flag_InvertSomething : 1;
		uint8 Flag_Transform : 1;
		uint8 Flag_Projected : 1;
		uint8 unk0 : 1;
		uint8 Flag_TextureStatic : 1;
		uint8 : 3;
	} flags;
	int8 priorityPlane;

	uint16 shader_id;
	uint16 m2SkinIndex;                // A duplicate entry of a submesh from the list above.
	uint16 geosetIndex;

	int16 colorIndex;                 // A Color out of the Colors-Block or -1 if none.
	uint16 materialIndex;              // The renderflags used on this texture-unit.
	uint16 materialLayer;              // Capped at 7 (see CM2Scene::BeginDraw)
	uint16 textureCount;               // 1 to 4. Also seems to be the number of m_DiffuseTextures to load, starting at the texture lookup in the next field (0x10).

	uint16 texture_Index;				// Index into Texture lookup table
	uint16 texture_CoordIndex;			// Index into the texture unit lookup table.
	uint16 texture_WeightIndex;			// Index into transparency lookup table.
	uint16 texture_TransformIndex;		// Index into uvanimation lookup table. 
};

struct SM2_SkinProfile
{
	M2Array<uint16>				vertices;
	M2Array<uint16>				indices;
	M2Array<SM2_SkinBones>		bones;
	M2Array<SM2_SkinSection>	submeshes;
	M2Array<SM2_SkinBatch>		batches;
	uint32						boneCountMax;
};
#include __PACK_END