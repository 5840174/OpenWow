#pragma once

#include "MDX_Skin_Batch.h"

#include "../shared/pack_begin.h"

struct M2SkinSection
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
	vec3 centerPosition;        // Average position of all the vertices in the sub mesh.
};

struct M2SkinBatch
{
	struct
	{
		uint8 Flag_InvertSomething : 1;
		uint8 Flag_Transform : 1;
		uint8 Flag_Projected : 1;
		uint8 unk0 : 1;
		uint8 Flag_TextureStatic : 1;
		uint8 : 3;
	} flags;
	int8 priorityPlane;
	
	uint16 shader_id;                  // See below.
	uint16 m2SkinIndex;                // A duplicate entry of a submesh from the list above.
	uint16 geosetIndex;                // See below.
	
	uint16 colorIndex;                 // A Color out of the Colors-Block or -1 if none.
	uint16 materialIndex;              // The renderflags used on this texture-unit.
	uint16 materialLayer;              // Capped at 7 (see CM2Scene::BeginDraw)
	uint16 textureCount;               // 1 to 4. Also seems to be the number of m_DiffuseTextures to load, starting at the texture lookup in the next field (0x10).
	
	uint16 texture_Index;          // Index into R_Texture lookup table
	uint16 texture_CoordIndex;
	uint16 texture_WeightIndex;    // Index into transparency lookup table.
	uint16 texture_TransformIndex; // Index into uvanimation lookup table. 
};

struct M2SkinBones
{
	uint8 index[4];
};

// GENERAL HEADER

struct M2SkinProfile
{
	M2Array<uint16> vertices;
	M2Array<uint16> indices;
	M2Array<M2SkinBones> bones;
	M2Array<M2SkinSection> submeshes;
	M2Array<M2SkinBatch> batches;

	uint32 boneCountMax;
};


#include "../shared/pack_end.h"

// Skin is a part of model

class Model_Skin
{
public:
	Model_Skin(MDX* _model, File& f, M2SkinProfile& view);
	~Model_Skin();

	void Draw();

private:
	MDX*                     m_Parent;
	vector<MDX_Skin_Batch*>  m_Batches;

    R_GeometryInfo*          __geom;
	bool*                    showGeosets;
};