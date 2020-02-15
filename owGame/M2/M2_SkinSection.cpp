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
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
{
	m_Properties = (ShaderM2GeometryProperties*)_aligned_malloc(sizeof(ShaderM2GeometryProperties), 16);
	(*m_Properties) = ShaderM2GeometryProperties();

	m_PropertiesBuffer = RenderDevice.GetObjectsFactory().CreateConstantBuffer(nullptr, sizeof(ShaderM2GeometryProperties));

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
		for (size_t i = 0; i < 4; i++)
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

	if (m_SkinSectionProto.boneCount > 0)
	{
		m_BonesList.resize(m_SkinSectionProto.boneCount);
		m_StructuredBuffer = m_RenderDevice.GetObjectsFactory().CreateStructuredBuffer(m_BonesList, CPUAccess::Write);
	}
}

CM2_SkinSection::~CM2_SkinSection()
{
	_aligned_free(m_Properties);
}

void CM2_SkinSection::UpdateGeometryProps(const RenderEventArgs& RenderEventArgs, const CM2_Base_Instance * M2Instance)
{
	M2Instance->getAnimator()->Update(RenderEventArgs.TotalTime, RenderEventArgs.DeltaTime);

	bool isAnimated = m_M2Model.getSkeleton()->hasBones() && m_M2Model.m_IsAnimated;
	/*m_Properties->gIsAnimated = isAnimated ? 1 : 0;
	if (isAnimated)
	{
		m_Properties->gBonesMaxInfluences = m_SkinSectionProto.boneInfluences;

		for (uint16 i = 0; i < m_SkinSectionProto.boneCount; i++)
			m_M2Model.getSkeleton()->getBoneLookup(m_SkinSectionProto.bonesStartIndex + i)->SetNeedCalculate();

		for (uint16 i = 0; i < m_SkinSectionProto.boneCount; i++)
			m_M2Model.getSkeleton()->getBoneLookup(m_SkinSectionProto.bonesStartIndex + i)->calcMatrix(M2Instance->getAnimator()->getSequenceIndex(), M2Instance->getAnimator()->getCurrentTime(), static_cast<uint32>(RenderEventArgs.TotalTime));

		for (uint16 i = 0; i < m_SkinSectionProto.boneCount; i++)
			m_M2Model.getSkeleton()->getBoneLookup(m_SkinSectionProto.bonesStartIndex + i)->calcBillboard(RenderEventArgs.Camera->GetViewMatrix(), M2Instance->GetWorldTransfom());

		for (uint16 i = 0; i < m_SkinSectionProto.boneCount; i++)
		{
			_ASSERT(m_M2Model.getSkeleton()->isLookupBoneCorrect(m_SkinSectionProto.bonesStartIndex + i));
			m_BonesList[i] = m_M2Model.getSkeleton()->getBoneLookup(m_SkinSectionProto.bonesStartIndex + i)->getTransformMatrix();
		}

		m_StructuredBuffer->Set(m_BonesList);
	}*/

	m_PropertiesBuffer->Set(m_Properties, sizeof(ShaderM2GeometryProperties));
}

const std::shared_ptr<IConstantBuffer>& CM2_SkinSection::GetGeometryPropsBuffer() const
{
	return m_PropertiesBuffer;
}

const std::shared_ptr<IStructuredBuffer>& CM2_SkinSection::GetGeometryBonesBuffer() const
{
	return m_StructuredBuffer;
}
