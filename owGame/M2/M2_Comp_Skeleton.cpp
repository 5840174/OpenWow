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

			std::shared_ptr<CM2_Part_Bone> bone = std::make_shared<CM2_Part_Bone>(m_M2Object, File, m2Bone);
			m_Bones.push_back(bone);
		}

		for (uint32 i = 0; i < M2Header.bones.size; i++)
			m_Bones[i]->SetParentBoneInternal(*this);

		m_HasBones = true;
	}

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

		_ASSERT(M2Header.gameBonesLookup.size <= M2_GameBoneType::Count);
	}
}
