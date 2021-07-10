#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_SkeletonComponent.h"

CM2SkeletonComponent::CM2SkeletonComponent(const CM2_Base_Instance& OwnerNode)
	: CSceneNodeComponentBase(OwnerNode)
{
	for (const auto& m2Bone : OwnerNode.GetM2().getSkeleton().GetBones())
		m_Bones.push_back(MakeShared(CM2SkeletonBone, m2Bone));

	for (const auto& bone : m_Bones)
		bone->SetParentAndChildsInternals(m_Bones);
}

CM2SkeletonComponent::~CM2SkeletonComponent()
{
}

std::vector<glm::mat4> CM2SkeletonComponent::CreatePose(size_t BoneStartIndex, size_t BonesCount) const
{
	std::vector<glm::mat4> result;
	result.reserve(BonesCount);
	for (size_t i = BoneStartIndex; i < BoneStartIndex + BonesCount; i++)
		result.push_back(m_Bones[GetM2OwnerNode().GetM2().getSkeleton().getBoneLookupIndex(i)]->GetMatrix());
	return result;
}

//
// ISkeletonComponent3D
//
std::shared_ptr<CM2SkeletonBone> CM2SkeletonComponent::GetBone(size_t Index) const
{
	_ASSERT(Index < m_Bones.size());
	return m_Bones.at(Index);
}

void CM2SkeletonComponent::Update(const UpdateEventArgs & e)
{
	// THIS ACTION DISABLES ANIMATIONs
	//return;

	for (const auto& b : m_Bones)
		b->Reset();

	for (const auto& b : m_Bones)
		b->Calculate(GetM2OwnerNode(), e.CameraForCulling, static_cast<uint32>(e.TotalTime));

	// TODO: Fix me
	const_cast<CM2_Base_Instance*>(&GetM2OwnerNode())->UpdateAttachPositionAfterSkeletonUpdate();
}

//
// Protected
//
const CM2_Base_Instance& CM2SkeletonComponent::GetM2OwnerNode() const
{
	return dynamic_cast<const CM2_Base_Instance&>(GetOwnerNode());
}
