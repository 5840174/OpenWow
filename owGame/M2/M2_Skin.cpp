#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Skin.h"

// Additional
#include "M2_Part_Material.h"

CM2_Skin::CM2_Skin(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const M2& M2Model, const SM2_SkinProfile& M2SkinProfile)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_M2Model(M2Model)
	, m_M2SkinProfile(M2SkinProfile)
{}

CM2_Skin::~CM2_Skin()
{
}

void CM2_Skin::Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File, const std::vector<SM2_Vertex>& Vertices)
{
	// Skin data
	const uint16*				t_verticesIndexes = (const uint16*)(File->getData() + m_M2SkinProfile.vertices.offset);
	const uint16*				t_indexesIndexes = (const uint16*)(File->getData() + m_M2SkinProfile.indices.offset);
	const SM2_SkinBones*		t_bonesIndexes = (const SM2_SkinBones*)(File->getData() + m_M2SkinProfile.bones.offset);
	const SM2_SkinSection*	t_sections = (const SM2_SkinSection*)(File->getData() + m_M2SkinProfile.submeshes.offset);

	_ASSERT(m_M2SkinProfile.vertices.size == m_M2SkinProfile.bones.size);

	for (uint32 sectionIndex = 0; sectionIndex < m_M2SkinProfile.submeshes.size; sectionIndex++)
	{
		const SM2_SkinSection& sectionProto = t_sections[sectionIndex];

		std::vector<SM2_Vertex> vertexes;
		for (uint32 vert = sectionProto.vertexStart; vert < sectionProto.vertexStart + sectionProto.vertexCount; vert++)
		{
			SM2_Vertex newVertex = Vertices[t_verticesIndexes[vert]];
			for (uint32 bone = 0; bone < sectionProto.boneInfluences; ++bone)
			{
				uint8 boneIndex = t_bonesIndexes[vert].index[bone];
				newVertex.bone_indices[bone] = boneIndex;
				_ASSERT(boneIndex < sectionProto.boneCount);
			}
			vertexes.push_back(newVertex);
		}

		std::vector<uint16> indexes;
		for (uint16 ind = sectionProto.indexStart; ind < sectionProto.indexStart + sectionProto.indexCount; ind++)
		{
			uint16 index = t_indexesIndexes[ind];
			_ASSERT(index >= sectionProto.vertexStart);
			_ASSERT(index < sectionProto.vertexStart + sectionProto.vertexCount);
			indexes.push_back(index - sectionProto.vertexStart);
		}

		m_Sections.push_back(std::make_shared<CM2_SkinSection>(m_RenderDevice, m_M2Model, sectionIndex, sectionProto, vertexes, indexes));
	}

	//--

	uint32	t_bonesMax = m_M2SkinProfile.boneCountMax;
	//_ASSERT(t_bonesMax == 256);
	//Log::Warn("t_bonesMax = %d", t_bonesMax);


	// BATCHES


	const SM2_SkinBatch* skinBatchesProtos = (const SM2_SkinBatch*)(File->getData() + m_M2SkinProfile.batches.offset);
	for (uint32 i = 0; i < m_M2SkinProfile.batches.size; i++)
	{
		std::shared_ptr<CM2_Skin_Batch> skinBatchObject = std::make_shared<CM2_Skin_Batch>(m_BaseManager, m_RenderDevice, m_M2Model, skinBatchesProtos[i]);
		m_Batches.push_back(skinBatchObject);

		auto& ttIter = m_TTT.find(m_Sections[skinBatchesProtos[i].skinSectionIndex]);
		if (ttIter == m_TTT.end())
		{
			m_TTT.insert(std::make_pair(m_Sections[skinBatchesProtos[i].skinSectionIndex], std::vector<std::shared_ptr<CM2_Skin_Batch>>({ skinBatchObject })));
		}
		else
		{
			ttIter->second.push_back(skinBatchObject);
		}

		//std::shared_ptr<IModel> model = m_RenderDevice.GetObjectsFactory().CreateModel();
		//model->AddConnection(skinBatchObject, m_Sections[skinBatchesProtos[i].skinSectionIndex]);
		//m_Models.push_back(model);
	}

	//std::sort(m_Batches.begin(), m_Batches.end(), [](const std::shared_ptr<CM2_Skin_Batch> left, const std::shared_ptr<CM2_Skin_Batch> right)
	//{
	//	return left->m_PriorityPlan < right->m_PriorityPlan;
	//});
}

void CM2_Skin::Accept(IVisitor * visitor)
{
	visitor->Visit(this);
}
