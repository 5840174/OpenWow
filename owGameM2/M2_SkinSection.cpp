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

CM2_SkinSection::CM2_SkinSection(IBaseManager* BaseManager, const std::weak_ptr<const M2> M2Model, const uint16 _index, const SM2_SkinSection& _proto, const std::vector<SM2_Vertex>& _vertexes, const std::vector<uint16>& _indexes) :
	m_Index(_index),
	m_Proto(_proto),
	m_ParentM2(M2Model)
{
	std::vector<vec3> verts;
	std::vector<SM2_Vertex_BoneWeight> weights;
	std::vector<SM2_Vertex_BoneIndex> indexes;
	std::vector<vec3> normals;
	std::vector<vec2> tex0;
	std::vector<vec2> tex1;

	for (const auto& it : _vertexes)
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

	std::shared_ptr<IBuffer> VB_Vertexes = BaseManager->GetManager<IRenderDevice>()->CreateVoidVertexBuffer(verts.data(), verts.size(), 0, sizeof(vec3));
	std::shared_ptr<IBuffer> VB_BoneWeights = BaseManager->GetManager<IRenderDevice>()->CreateVoidVertexBuffer(weights.data(), weights.size(), 0, sizeof(SM2_Vertex_BoneWeight));
	std::shared_ptr<IBuffer> VB_BoneIndices = BaseManager->GetManager<IRenderDevice>()->CreateVoidVertexBuffer(indexes.data(), indexes.size(), 0, sizeof(SM2_Vertex_BoneIndex));
	std::shared_ptr<IBuffer> VB_Normals = BaseManager->GetManager<IRenderDevice>()->CreateVoidVertexBuffer(normals.data(), normals.size(), 0, sizeof(vec3));
	std::shared_ptr<IBuffer> VB_TextureCoords0 = BaseManager->GetManager<IRenderDevice>()->CreateVoidVertexBuffer(tex0.data(), tex0.size(), 0, sizeof(vec2));
	std::shared_ptr<IBuffer> VB_TextureCoords1 = BaseManager->GetManager<IRenderDevice>()->CreateVoidVertexBuffer(tex1.data(), tex1.size(), 0, sizeof(vec2));
	std::shared_ptr<IBuffer> IB_Indexes = BaseManager->GetManager<IRenderDevice>()->CreateIndexBuffer(_indexes);

	m_Mesh = BaseManager->GetManager<IRenderDevice>()->CreateMesh();
	m_Mesh->AddVertexBuffer(BufferBinding("POSITION", 0), VB_Vertexes);
	m_Mesh->AddVertexBuffer(BufferBinding("BLENDWEIGHT", 0), VB_BoneWeights);
	m_Mesh->AddVertexBuffer(BufferBinding("BLENDINDICES", 0), VB_BoneIndices);
	m_Mesh->AddVertexBuffer(BufferBinding("NORMAL", 0), VB_Normals);
	m_Mesh->AddVertexBuffer(BufferBinding("TEXCOORD", 0), VB_TextureCoords0);
	m_Mesh->AddVertexBuffer(BufferBinding("TEXCOORD", 1), VB_TextureCoords1);
	m_Mesh->SetIndexBuffer(IB_Indexes);
}
