#include "stdafx.h"

// Include
#include "M2_Builder.h"

// General
#include "M2_Skin_Builder.h"

// Additional
#include "ShaderResolver.h"

CM2_Skin_Builder::CM2_Skin_Builder(IBaseManager* BaseManager, const CM2_Builder& _m2Builder, std::weak_ptr<M2> _model, const SM2_SkinProfile& _skinProto, std::shared_ptr<CM2_Skin> _skin, std::shared_ptr<IFile> _file)
	: m_M2Builder(_m2Builder)
	, m_ParentM2(_model)
    , m_SkinProfile(_skinProto)
    , m_Skin(_skin)
    , m_F(_file)
	, m_BaseManager(BaseManager)
{}

void CM2_Skin_Builder::Load()
{
	Step1LoadProfile();
	Step2InitBatches();
}

void CM2_Skin_Builder::Step1LoadProfile()
{
	std::shared_ptr<IFile> F = m_F.lock();
	_ASSERT(F != nullptr);

	std::shared_ptr<CM2_Skin> Skin = m_Skin.lock();
	_ASSERT(Skin != nullptr);

	//F->readBytes(&m_SkinProfile, sizeof(SM2_SkinProfile));

	// Skin data
	uint16*				t_verticesIndexes	= (uint16*)				(F->getData() + m_SkinProfile.vertices.offset);
	uint16*				t_indexesIndexes	= (uint16*)				(F->getData() + m_SkinProfile.indices.offset);
	SM2_SkinBones*		t_bonesIndexes		= (SM2_SkinBones*)		(F->getData() + m_SkinProfile.bones.offset);
	SM2_SkinSection*	t_sections			= (SM2_SkinSection*)	(F->getData() + m_SkinProfile.submeshes.offset);
	SM2_SkinBatch*		t_Batches			= (SM2_SkinBatch*)		(F->getData() + m_SkinProfile.batches.offset);

	_ASSERT(m_SkinProfile.vertices.size == m_SkinProfile.bones.size);

	for (uint32 sectionIndex = 0; sectionIndex < m_SkinProfile.submeshes.size; sectionIndex++)
	{
		const SM2_SkinSection& section = t_sections[sectionIndex];
		
		std::vector<SM2_Vertex> vertexes;
		for (uint32 vert = section.vertexStart; vert < section.vertexStart + section.vertexCount; vert++)
		{
			SM2_Vertex newVertex = m_M2Builder.m_Vertexes[t_verticesIndexes[vert]];
			for (uint32 bone = 0; bone < section.boneInfluences; ++bone)
			{
				uint8 boneIndex = t_bonesIndexes[vert].index[bone];
				newVertex.bone_indices[bone] = boneIndex;
				_ASSERT(boneIndex < section.boneCount);
			}
			vertexes.push_back(newVertex);
		}

		std::vector<uint16> indexes;
		for (uint16 ind = section.indexStart; ind < section.indexStart + section.indexCount; ind++)
		{
			uint16 index = t_indexesIndexes[ind];
			_ASSERT(index >= section.vertexStart);
			_ASSERT(index < section.vertexStart + section.vertexCount);
			indexes.push_back(index - section.vertexStart);
		}

		std::shared_ptr<CM2_SkinSection> skinSection = std::make_shared<CM2_SkinSection>(m_BaseManager, m_ParentM2, sectionIndex, section, vertexes, indexes);
		Skin->m_Sections.push_back(skinSection);
	}

	//--

	for (uint32 i = 0; i < m_SkinProfile.batches.size; i++)
	{
		m_SkinBatches.push_back(t_Batches[i]);
	}

	uint32	t_bonesMax = m_SkinProfile.boneCountMax;
	//_ASSERT(t_bonesMax == 256);
	//Log::Warn("t_bonesMax = %d", t_bonesMax);
}

void CM2_Skin_Builder::Step2InitBatches()
{
	std::shared_ptr<CM2_Skin> Skin = m_Skin.lock();
	_ASSERT(Skin != nullptr);

	const std::shared_ptr<const CM2_Comp_Materials> materials = m_ParentM2.lock()->getMaterials();

	for (const auto& it : m_SkinBatches)
	{
		std::shared_ptr<CM2_SkinSection> skinSection = Skin->m_Sections[it.skinSectionIndex];

		std::shared_ptr<CM2_Skin_Batch> batch = std::make_shared<CM2_Skin_Batch>(m_BaseManager, m_ParentM2, skinSection->getMesh());

		//Log::Green("Shader = %d", it.shader_id);
        batch->newShader = it.shader_id;
		//batch->newShader = GetPixel(it);

		// Geometry data
		batch->m_PriorityPlan = it.priorityPlane;
		batch->m_SkinSection = skinSection;

		// Get classes
		batch->m_Material = (materials->GetMaterial(it.materialIndex));

		// Color
		if (it.colorIndex != -1)
		{
			batch->m_Color = (materials->GetColor(it.colorIndex));
		}

		// Textures
		for (uint32 i = 0; i < it.textureCount; i++)
		{
			batch->m_Textures.push_back((materials->GetTexture(it.texture_Index + i)));
		}

		// Texture unit
		if (it.texture_CoordIndex != -1)
		{
			batch->m_TextureUnit = materials->m_TexturesUnitLookup[it.texture_CoordIndex];
		}

		// Texture weight
		if (it.texture_WeightIndex != -1)
		{
			batch->m_TextureWeight = (materials->m_TextureWeights[it.texture_WeightIndex]);
		}

		// Texture transfowm
		if (it.flags.TextureStatic == false)
		{
			if (it.texture_TransformIndex != -1)
			{
				int16 index = materials->m_TexturesTransformLookup[it.texture_TransformIndex];
				if (index != -1)
				{
					batch->m_TextureTransform = (materials->GetTextureTransform(it.texture_TransformIndex));
				}
			}
		}

		batch->PostInit();
		Skin->m_Batches.push_back(batch);
	}

	std::sort(Skin->m_Batches.begin(), Skin->m_Batches.end(), [](const std::shared_ptr<CM2_Skin_Batch> left, const std::shared_ptr<CM2_Skin_Batch> right)
	{
		return left->getPriorityPlan() < right->getPriorityPlan();
	});
}