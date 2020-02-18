#include "stdafx.h"

// Include
#include "M2_Builder.h"

// General
#include "M2_Skin_Builder.h"

// Additional
#include "ShaderResolver.h"

CM2_Skin_Builder::CM2_Skin_Builder(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CM2_Builder& M2Builder, const M2& M2Model, const SM2_SkinProfile& SkinProfileProto, std::shared_ptr<IFile> _file)
	: m_M2Builder(M2Builder)
	, m_M2Model(M2Model)
    , m_SkinProfileProto(SkinProfileProto)
    , m_F(_file)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
{}

CM2_Skin_Builder::~CM2_Skin_Builder()
{
}

std::shared_ptr<CM2_Skin> CM2_Skin_Builder::Load()
{
	std::shared_ptr<CM2_Skin> skin = std::make_shared<CM2_Skin>(m_RenderDevice, m_M2Model);
	Step1LoadProfile(skin);
	Step2InitBatches(skin);
	return skin;
}

void CM2_Skin_Builder::Step1LoadProfile(const std::shared_ptr<CM2_Skin>& M2SkinObject)
{
	std::shared_ptr<IFile> F = m_F.lock();
	_ASSERT(F != nullptr);

	//F->readBytes(&m_SkinProfile, sizeof(SM2_SkinProfile));

	// Skin data
	uint16*				t_verticesIndexes	= (uint16*)				(F->getData() + m_SkinProfileProto.vertices.offset);
	uint16*				t_indexesIndexes	= (uint16*)				(F->getData() + m_SkinProfileProto.indices.offset);
	SM2_SkinBones*		t_bonesIndexes		= (SM2_SkinBones*)		(F->getData() + m_SkinProfileProto.bones.offset);
	SM2_SkinSection*	t_sections			= (SM2_SkinSection*)	(F->getData() + m_SkinProfileProto.submeshes.offset);

	_ASSERT(m_SkinProfileProto.vertices.size == m_SkinProfileProto.bones.size);

	for (uint32 sectionIndex = 0; sectionIndex < m_SkinProfileProto.submeshes.size; sectionIndex++)
	{
		const SM2_SkinSection& sectionProto = t_sections[sectionIndex];
		
		std::vector<SM2_Vertex> vertexes;
		for (uint32 vert = sectionProto.vertexStart; vert < sectionProto.vertexStart + sectionProto.vertexCount; vert++)
		{
			SM2_Vertex newVertex = m_M2Builder.m_Vertexes[t_verticesIndexes[vert]];
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

		M2SkinObject->m_Sections.push_back(std::make_shared<CM2_SkinSection>(m_RenderDevice, m_M2Model, sectionIndex, sectionProto, vertexes, indexes));
	}

	//--

	uint32	t_bonesMax = m_SkinProfileProto.boneCountMax;
	//_ASSERT(t_bonesMax == 256);
	//Log::Warn("t_bonesMax = %d", t_bonesMax);
}

void CM2_Skin_Builder::Step2InitBatches(const std::shared_ptr<CM2_Skin>& M2SkinObject)
{
	std::shared_ptr<IFile> F = m_F.lock();
	_ASSERT(F != nullptr);

	SM2_SkinBatch* skinBatchesProtos = (SM2_SkinBatch*)(F->getData() + m_SkinProfileProto.batches.offset);
	for (uint32 i = 0; i < m_SkinProfileProto.batches.size; i++)
	{
		std::shared_ptr<CM2_Skin_Batch> skinBatchObject = std::make_shared<CM2_Skin_Batch>(m_BaseManager, m_RenderDevice, m_M2Model, skinBatchesProtos[i]);
		M2SkinObject->m_Batches.push_back(skinBatchObject);

		auto& ttIter = M2SkinObject->m_TTT.find(M2SkinObject->m_Sections[skinBatchesProtos[i].skinSectionIndex]);
		if (ttIter == M2SkinObject->m_TTT.end())
		{
			M2SkinObject->m_TTT.insert(std::make_pair(M2SkinObject->m_Sections[skinBatchesProtos[i].skinSectionIndex], std::vector<std::shared_ptr<CM2_Skin_Batch>>({ skinBatchObject })));
		}
		else
		{
			ttIter->second.push_back(skinBatchObject);
		}

		//std::shared_ptr<IModel> model = m_RenderDevice.GetObjectsFactory().CreateModel();
		//model->AddConnection(skinBatchObject, M2SkinObject->m_Sections[skinBatchesProtos[i].skinSectionIndex]);
		//M2SkinObject->m_Models.push_back(model);
	}

	//std::sort(M2SkinObject->m_Batches.begin(), M2SkinObject->m_Batches.end(), [](const std::shared_ptr<CM2_Skin_Batch> left, const std::shared_ptr<CM2_Skin_Batch> right)
	//{
	//	return left->m_PriorityPlan < right->m_PriorityPlan;
	//});
}