#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_SkinSection.h"

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


struct SM2_Vertex_BoneWeight
{
	float weights[4];
};

struct SM2_Vertex_BoneIndex
{
	uint32 indexes[4];
};

CM2_SkinSection::CM2_SkinSection(IRenderDevice& RenderDevice, const CM2& M2Model, const uint16 SkinSectionIndex, const SM2_SkinSection& SkinSectionProto, const std::vector<SM2_Vertex>& Vertexes, const std::vector<uint16>& Indexes)
	: GeometryProxie(RenderDevice.GetObjectsFactory().CreateGeometry())
	, m_SkinSectionIndex(SkinSectionIndex)
	, m_SkinSectionProto(SkinSectionProto)
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
{
	m_Properties = (ShaderM2GeometryProperties*)_aligned_malloc(sizeof(ShaderM2GeometryProperties), 16);
	(*m_Properties) = ShaderM2GeometryProperties();

	m_PropertiesBuffer = RenderDevice.GetObjectsFactory().CreateConstantBuffer(nullptr, sizeof(ShaderM2GeometryProperties));


	std::vector<SM2_Vertex_znEngine> zenonVertices;
	zenonVertices.resize(Vertexes.size());
	for (size_t i = 0; i < Vertexes.size(); i++)
	{
		const auto& m2Vertex = Vertexes[i];

		SM2_Vertex_znEngine vertex;
		vertex.pos = m2Vertex.pos;
		
		for (size_t j = 0; j < 4; j++)
		{
			_ASSERT(m2Vertex.bone_indices[j] < m_SkinSectionProto.boneCount);

			vertex.bone_weights.weights[j] = static_cast<float>(m2Vertex.bone_weights[j]) / 255.0f;
			vertex.bone_indices.indexes[j] = m2Vertex.bone_indices[j];
		}

		vertex.normal = m2Vertex.normal;
		vertex.tex_coords[0] = m2Vertex.tex_coords[0];
		vertex.tex_coords[1] = m2Vertex.tex_coords[1];

		zenonVertices[i] = vertex;
	}

	SetVertexBuffer(RenderDevice.GetObjectsFactory().CreateVertexBuffer(zenonVertices));
	SetIndexBuffer(RenderDevice.GetObjectsFactory().CreateIndexBuffer(Indexes));

	if (m_SkinSectionProto.boneCount > 0)
	{
		m_BonesList.resize(m_SkinSectionProto.boneCount);
		m_BonesBuffer = m_RenderDevice.GetObjectsFactory().CreateStructuredBuffer(m_BonesList, EAccess::CPUWrite);
	}
}

CM2_SkinSection::~CM2_SkinSection()
{
	_aligned_free(m_Properties);
}

void CM2_SkinSection::UpdateGeometryProps(const RenderEventArgs& RenderEventArgs, const CM2_Base_Instance * M2Instance)
{
	//M2Instance->getAnimator()->Update(RenderEventArgs.TotalTime, RenderEventArgs.DeltaTime);

	m_Properties->gIsAnimated = m_M2Model.getSkeleton().hasBones() && m_M2Model.isAnimated();
	if (m_Properties->gIsAnimated)
	{
		m_Properties->gStartBoneIndex = m_SkinSectionProto.bonesStartIndex;
		m_Properties->gBonesMaxInfluences = m_SkinSectionProto.boneInfluences;

		M2Instance->getSkeletonComponent()->CreatePose(m_SkinSectionProto.bonesStartIndex, m_SkinSectionProto.boneCount, &m_BonesList);
		_ASSERT(m_BonesList.size() == m_SkinSectionProto.boneCount);
		m_BonesBuffer->Set(m_BonesList.data(), sizeof(glm::mat4), m_SkinSectionProto.boneCount);
	}

	m_PropertiesBuffer->Set(m_Properties, sizeof(ShaderM2GeometryProperties));
}

const std::shared_ptr<IConstantBuffer>& CM2_SkinSection::GetGeometryPropsBuffer() const
{
	return m_PropertiesBuffer;
}

const std::shared_ptr<IStructuredBuffer>& CM2_SkinSection::GetGeometryBonesBuffer() const
{
	return m_BonesBuffer;
}
