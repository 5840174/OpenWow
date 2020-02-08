#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"
#include "M2_Skin_Builder.h"

// General
#include "M2_SkinSection.h"

struct SM2_Vertex_BoneWeight
{
	float weights[4];
};

struct SM2_Vertex_BoneIndex
{
	uint32 indexes[4];
};

CM2_SkinSection::CM2_SkinSection(IRenderDevice& RenderDevice, const M2& M2Model, const uint16 SkinSectionIndex, const SM2_SkinSection& SkinSectionProto, const std::vector<SM2_Vertex>& Vertexes, const std::vector<uint16>& Indexes)
	: GeometryProxie(RenderDevice.GetObjectsFactory().CreateGeometry())
	, m_SkinSectionIndex(SkinSectionIndex)
	, m_SkinSectionProto(SkinSectionProto)
	, m_M2Model(M2Model)
{
	std::vector<vec3> verts;
	std::vector<SM2_Vertex_BoneWeight> weights;
	std::vector<SM2_Vertex_BoneIndex> indexes;
	std::vector<vec3> normals;
	std::vector<vec2> tex0;
	std::vector<vec2> tex1;

	for (const auto& it : Vertexes)
	{
		SM2_Vertex_BoneWeight m2_weights;
		SM2_Vertex_BoneIndex m2_indexes;
		for (uint8 i = 0; i < 4; i++)
		{
			m2_weights.weights[i] = static_cast<float>(it.bone_weights[i]) / 255.0f;
			m2_indexes.indexes[i] = it.bone_indices[i];
		}

		verts.push_back(it.pos);
		weights.push_back(m2_weights);
		indexes.push_back(m2_indexes);
		normals.push_back(it.normal);
		tex0.push_back(it.tex_coords[0]);
		tex1.push_back(it.tex_coords[1]);
	}

	AddVertexBuffer(BufferBinding("POSITION", 0), RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(verts.data(), verts.size(), 0, sizeof(vec3)));
	AddVertexBuffer(BufferBinding("BLENDWEIGHT", 0), RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(weights.data(), weights.size(), 0, sizeof(SM2_Vertex_BoneWeight)));
	AddVertexBuffer(BufferBinding("BLENDINDICES", 0), RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(indexes.data(), indexes.size(), 0, sizeof(SM2_Vertex_BoneIndex)));
	AddVertexBuffer(BufferBinding("NORMAL", 0), RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(normals.data(), normals.size(), 0, sizeof(vec3)));
	AddVertexBuffer(BufferBinding("TEXCOORD", 0), RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(tex0.data(), tex0.size(), 0, sizeof(vec2)));
	AddVertexBuffer(BufferBinding("TEXCOORD", 1), RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(tex1.data(), tex1.size(), 0, sizeof(vec2)));
	SetIndexBuffer(RenderDevice.GetObjectsFactory().CreateIndexBuffer(Indexes));
}

CM2_SkinSection::~CM2_SkinSection()
{
}
