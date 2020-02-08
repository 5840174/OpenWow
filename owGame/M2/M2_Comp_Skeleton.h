#pragma once

#include "M2_Part_Bone.h"

// FORWARD BEGIN
class CM2_Builder;
// FORWARD END

class CM2_Comp_Skeleton
{
	friend class CM2_Builder;
public:
	CM2_Comp_Skeleton();

	void calc(uint16 anim, uint32 time, uint32 globalTime, cmat4 _viewMat, cmat4 _worldMat);

	const bool hasBones() const { return m_HasBones; }
	const bool isAnimBones() const { return m_IsAnimBones; }
	const bool isBillboard() const { return m_IsBillboard; }

public:
	std::shared_ptr<CM2_Part_Bone> getBoneDirect(uint32 _index) const
	{
		_ASSERT(_index < static_cast<uint32>(m_Bones.size()));
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
		_ASSERT(newIndex != -1);
		_ASSERT(newIndex < static_cast<int16>(m_Bones.size()));
		return (m_Bones[newIndex]);
	}
	std::shared_ptr<CM2_Part_Bone> getGameBone(M2_GameBoneType::List _type) const
	{
		if (_type >= static_cast<int16>(m_GameBonesLookup.size()))
		{
			//Log::Warn("M2[%s]: getGameBone [%d] not found in Lookup[%d]", m_ParentM2->getFilenameWithoutExt().c_str(), _type, m_GameBonesLookup.size());
			return nullptr;
		}
		int16 newIndex = m_GameBonesLookup[_type];
		_ASSERT(newIndex != -1);
		_ASSERT(newIndex < static_cast<int16>(m_Bones.size()));
		return (m_Bones[newIndex]);
	}


private:
	std::vector<std::shared_ptr<CM2_Part_Bone>>			m_Bones;
	std::vector<int16>					m_BonesLookup;
	std::vector<int16>					m_GameBonesLookup;
	bool								m_HasBones;
	bool								m_IsAnimBones;
	bool								m_IsBillboard;
};