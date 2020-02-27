#pragma once

// M2
#include "M2_Headers.h"

// M2 skeleton
#include "M2_Part_Bone.h"

// FORWARD BEGIN
class M2;
// FORWARD END

class CM2_Comp_Skeleton
{
public:
	CM2_Comp_Skeleton(const M2& M2Object);
	virtual ~CM2_Comp_Skeleton();

	void Load(const SM2_Header& M2Header, const std::shared_ptr<IFile>& File);

	const bool hasBones() const { return m_HasBones; }
	const bool isAnimBones() const { return m_IsAnimBones; }
	const bool isBillboard() const { return m_IsBillboard; }

	std::vector<glm::mat4> CreatePose(size_t BoneStartIndex, size_t BonesCount, uint16 anim, uint32 time, uint32 globalTime) const;

public: // Loops & Sequences
	const std::vector<SM2_Loop>& getGlobalLoops() const
	{
		return m_GlobalLoops;
	}
	const SM2_Sequence& getSequenceDirect(uint32 _index) const // TODO: Delete me!
	{
		_ASSERT(_index != -1 && _index < static_cast<uint32>(m_Sequences.size()));
		return m_Sequences[_index];
	}
	const size_t getSequencesCount() const // TODO: Delete me!
	{
		return m_Sequences.size();
	}
	const SM2_Sequence& getSequenceLookup(uint32 _index) const
	{
		_ASSERT(_index < m_SequencesLookup.size());
		int16 newIndex = m_SequencesLookup[_index];
		_ASSERT(newIndex != -1 && newIndex < static_cast<int16>(m_Sequences.size()));
		return m_Sequences[newIndex];
	}
private:
	std::vector<SM2_Loop>      m_GlobalLoops;
	std::vector<SM2_Sequence>  m_Sequences;
	std::vector<int16>         m_SequencesLookup;

public:
	std::shared_ptr<const CM2_Part_Bone> getBoneDirect(int16 _index) const
	{
		_ASSERT(_index != -1 && _index < static_cast<int16>(m_Bones.size()));
		return (m_Bones[_index]);
	}
	bool isLookupBoneCorrect(uint32 _index) const
	{
		int16 newIndex = m_BonesLookup[_index];
		return (newIndex != -1) && (newIndex < static_cast<int16>(m_Bones.size()));
	}
	std::shared_ptr<CM2_Part_Bone> getBoneLookup(uint32 _index) const
	{
		if (_index >= static_cast<uint32>(m_BonesLookup.size()))
		{
			//Log::Warn("M2[%s]: getBone [%d] not found in Lookup[%d]", m_FileName.c_str(), _index, m_Header.bonesLookup.size);
			return nullptr;
		}
		int16 newIndex = m_BonesLookup[_index];
		_ASSERT(newIndex != -1 && newIndex < static_cast<int16>(m_Bones.size()));
		return (m_Bones[newIndex]);
	}
	std::shared_ptr<const CM2_Part_Bone> getGameBone(M2_GameBoneType _type) const
	{
		if (static_cast<int16>(_type) >= static_cast<int16>(m_GameBonesLookup.size()))
		{
			//Log::Warn("M2[%s]: getGameBone [%d] not found in Lookup[%d]", m_ParentM2->getFilenameWithoutExt().c_str(), _type, m_GameBonesLookup.size());
			return nullptr;
		}
		int16 newIndex = m_GameBonesLookup[static_cast<int16>(_type)];
		_ASSERT(newIndex != -1 && newIndex < static_cast<int16>(m_Bones.size()));
		return (m_Bones[newIndex]);
	}
private:
	std::vector<std::shared_ptr<CM2_Part_Bone>>  m_Bones;
	std::vector<int16>                           m_BonesLookup;
	std::vector<int16>                           m_GameBonesLookup;

	bool                                         m_HasBones;
	bool                                         m_IsAnimBones;
	bool                                         m_IsBillboard;

private:
	const M2& m_M2Object;
};