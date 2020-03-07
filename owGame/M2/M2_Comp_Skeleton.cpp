#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Comp_Skeleton.h"

CM2_Comp_Skeleton::CM2_Comp_Skeleton(const CM2& M2Object)
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

			m_Bones.push_back(SM2_Part_Bone_Wrapper(m_M2Object, File, m2Bone));
		}

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
	_ASSERT(M2Header.gameBonesLookup.size < 28 /* TODO: HARDCODED */);
	if (M2Header.gameBonesLookup.size > 0)
	{
		const int16* GameBonesLookup = (const int16*)(File->getData() + M2Header.gameBonesLookup.offset);
		for (uint32 i = 0; i < M2Header.gameBonesLookup.size; i++)
			m_GameBonesLookup.push_back(GameBonesLookup[i]);
	}
}
