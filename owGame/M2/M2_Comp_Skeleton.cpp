#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Comp_Skeleton.h"

CM2_Comp_Skeleton::CM2_Comp_Skeleton(const M2& M2Object)
	: m_HasBones(false)
	, m_IsAnimBones(false)
	, m_IsBillboard(false)
	, m_M2Object(M2Object)
{
}

CM2_Comp_Skeleton::~CM2_Comp_Skeleton()
{
}

void CM2_Comp_Skeleton::Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File)
{
	if (M2Header.global_loops.size > 0)
	{
		const SM2_Loop* GlobalLoops = (const SM2_Loop*)(File->getData() + M2Header.global_loops.offset);
		for (uint32 i = 0; i < M2Header.global_loops.size; i++)
			m_GlobalLoops.push_back(GlobalLoops[i]);
	}

	// Sequences
	if (M2Header.sequences.size > 0)
	{
		const SM2_Sequence* Sequences = (const SM2_Sequence*)(File->getData() + M2Header.sequences.offset);
		for (uint32 i = 0; i < M2Header.sequences.size; i++)
			m_Sequences.push_back(Sequences[i]);
	}

	// Sequences Lookup
	if (M2Header.sequencesLookup.size > 0)
	{
		const int16* SequencesLookup = (const int16*)(File->getData() + M2Header.sequencesLookup.offset);
		for (uint32 i = 0; i < M2Header.sequencesLookup.size; i++)
			m_SequencesLookup.push_back(SequencesLookup[i]);
	}

	// Bones
	if (M2Header.bones.size > 0)
	{
		const SM2_Bone* m2Bones = (const SM2_Bone*)(File->getData() + M2Header.bones.offset);
		for (uint32 i = 0; i < M2Header.bones.size; i++)
		{
			const SM2_Bone& m2Bone = m2Bones[i];
			if (m2Bone.IsAnimated())
				m_IsAnimBones = true;

			if (m2Bone.IsBillboard())
				m_IsBillboard = true;

			std::shared_ptr<CM2_Part_Bone> bone = std::make_shared<CM2_Part_Bone>(m_M2Object, File, i, m2Bone);
			m_Bones.push_back(bone);
		}

		for (uint32 i = 0; i < M2Header.bones.size; i++)
			m_Bones[i]->SetParentBoneInternal(*this);

		m_HasBones = true;
	}

	//for (const auto& bb : m_Bones)
	//	if (bb->getParentBoneID_DirectArray() == -1)
	//		bb->Print(0);

	// Bones Lookup
	if (M2Header.bonesLookup.size > 0)
	{
		const int16* BonesLookup = (const int16*)(File->getData() + M2Header.bonesLookup.offset);
		for (uint32 i = 0; i < M2Header.bonesLookup.size; i++)
			m_BonesLookup.push_back(BonesLookup[i]);
	}

	// Game Bones Lookup
	if (M2Header.gameBonesLookup.size > 0)
	{
		const int16* GameBonesLookup = (const int16*)(File->getData() + M2Header.gameBonesLookup.offset);
		for (uint32 i = 0; i < M2Header.gameBonesLookup.size; i++)
			m_GameBonesLookup.push_back(GameBonesLookup[i]);

		_ASSERT(M2Header.gameBonesLookup.size < 28 /* TODO: HARDCODED */);
	}
}


struct CBone
{
	CBone()
		: isCalculated(false)
	{}

	int16 parentBoneIndex; // direct array
	glm::mat4 matrix;      // Self bone matrix

	bool isCalculated;
	glm::mat4 resultMatrix; // All parents + self matrix
};


glm::mat4 CalculateBone(std::unordered_map<int16, CBone>& Bones, CBone& Bone)
{
	if (Bone.isCalculated)
		return Bone.resultMatrix;

	if (Bone.parentBoneIndex != -1)
	{
		auto& it = Bones[Bone.parentBoneIndex];
		Bone.resultMatrix = CalculateBone(Bones, it) * Bone.matrix;
	}
	else
	{
		Bone.resultMatrix = Bone.matrix;
	}

	Bone.isCalculated = true;
	return Bone.resultMatrix;
}

std::vector<glm::mat4> CM2_Comp_Skeleton::CreatePose(size_t BoneStartIndex, size_t BonesCount, uint16 anim, uint32 time, uint32 globalTime) const
{
	_ASSERT((BoneStartIndex < m_BonesLookup.size()) && ((BoneStartIndex + BonesCount) < m_BonesLookup.size()));

	//auto rootBone = getGameBone(M2_GameBoneType::Root);
	//if (rootBone == nullptr)
	//	return std::vector<glm::mat4>({ glm::mat4(1.0f) });

	std::unordered_map<int16, CBone> calculatedMatrices;
	calculatedMatrices.reserve(m_Bones.size());

	for (size_t i = 0; i < m_Bones.size(); i++)
	{
		auto b = m_Bones[i];
		_ASSERT(b != nullptr);

		CBone bone;
		bone.parentBoneIndex = b->getParentBoneID_DirectArray();
		bone.matrix = b->calcMatrix(anim, time, globalTime);
		bone.resultMatrix = CalculateBone(calculatedMatrices, bone);

		calculatedMatrices.insert(std::make_pair(i, bone));
	}

	std::vector<glm::mat4> result;
	result.reserve(BonesCount);

	for (size_t i = BoneStartIndex; i < BoneStartIndex + BonesCount; i++)
	{
		result.push_back(calculatedMatrices[m_BonesLookup[i]].resultMatrix);
	}
	
	return result;
}
