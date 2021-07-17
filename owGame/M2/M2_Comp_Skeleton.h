#pragma once

// M2
#include "M2_Headers.h"

// M2 skeleton
#include "M2_Part_Bone.h"

// FORWARD BEGIN
class CM2;
// FORWARD END

class ZN_API CM2_Comp_Skeleton
{
public:
	CM2_Comp_Skeleton(const CM2& M2Object);
	virtual ~CM2_Comp_Skeleton();

	void Load(const SM2_Header& M2Header, const std::shared_ptr<IByteBuffer>& File);

	const bool hasBones() const { return m_HasBones; }
	const bool isAnimBones() const { return m_IsAnimBones; }
	const bool isBillboard() const { return m_IsBillboard; }


public: // Loops & Sequences
	const std::vector<SM2_Loop>& getGlobalLoops() const
	{
		return m_GlobalLoops;
	}
	const std::vector<SM2_Sequence>& GetSequences() const
	{
		return m_Sequences;
	}
	const SM2_Sequence& getSequenceLookup(size_t IndexIntoLookup) const
	{
		_ASSERT(IndexIntoLookup < m_SequencesLookup.size());
		int16 indexIntoDirect = m_SequencesLookup[IndexIntoLookup];
		_ASSERT(indexIntoDirect != -1 && indexIntoDirect < static_cast<int16>(m_Sequences.size()));
		return m_Sequences[indexIntoDirect];
	}
	const std::vector<std::shared_ptr<IFile>>& GetAnimFiles() const
	{
		return animFiles;
	}
private:
	std::vector<SM2_Loop>      m_GlobalLoops;
	std::vector<SM2_Sequence>  m_Sequences;
	std::vector<int16>         m_SequencesLookup;
	std::vector<std::shared_ptr<IFile>> animFiles;


public:
	const std::vector<CM2_Part_Bone>& GetBonesDirectArray() const
	{
		return m_Bones;
	}
	// Lookup bones
	bool isLookupBoneCorrect(uint32 IndexIntoLookup) const
	{
		int16 indexIntoDirect = m_BonesLookup[IndexIntoLookup];
		return (indexIntoDirect != -1) && (indexIntoDirect < static_cast<int16>(m_Bones.size()));
	}
	int16 getBoneLookupIndex(uint32 IndexIntoLookup) const
	{
		_ASSERT(static_cast<size_t>(IndexIntoLookup) < m_BonesLookup.size());
		return m_BonesLookup[static_cast<size_t>(IndexIntoLookup)];
	}
	const CM2_Part_Bone& getBoneLookup(uint32 IndexIntoLookup) const
	{
		_ASSERT(IndexIntoLookup < static_cast<uint32>(m_BonesLookup.size()));
		int16 indexIntoDirect = m_BonesLookup[IndexIntoLookup];
		_ASSERT(indexIntoDirect != -1 && indexIntoDirect < static_cast<int16>(m_Bones.size()));
		return m_Bones[indexIntoDirect];
	}
	// Game bones
	bool IsGameBoneIndexCorrect(uint32 IndexIntoLookup) const
	{
		int16 indexIntoDirect = m_GameBonesLookup[IndexIntoLookup];
		return (indexIntoDirect != -1) && (indexIntoDirect < static_cast<int16>(m_Bones.size()));
	}
	int16 GetGameBoneIndex(M2_GameBoneType _type) const
	{
		_ASSERT(static_cast<size_t>(_type) < m_GameBonesLookup.size());
		return m_GameBonesLookup[static_cast<size_t>(_type)];
	}
	const CM2_Part_Bone& getGameBone(M2_GameBoneType _type) const
	{
		int16 indexIntoDirect = GetGameBoneIndex(_type);
		_ASSERT(indexIntoDirect != -1 && indexIntoDirect < static_cast<int16>(m_Bones.size()));
		return m_Bones[indexIntoDirect];
	}
private:
	std::vector<CM2_Part_Bone>           m_Bones;
	std::vector<int16>                           m_BonesLookup;
	std::vector<int16>                           m_GameBonesLookup;

	bool                                         m_HasBones;
	bool                                         m_IsAnimBones;
	bool                                         m_IsBillboard;


private:
	const CM2& m_M2Object;
};
