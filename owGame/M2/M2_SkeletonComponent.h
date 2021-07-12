#pragma once

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

#include "M2_SkeletonBone.h"

class ZN_API CM2SkeletonComponent
	: public CSceneNodeComponentBase
{
public:
	ZN_OBJECTCLASS(cSceneNodeComponentM2SkeletonComponent);

	CM2SkeletonComponent(const CM2_Base_Instance& OwnerNode);
	virtual ~CM2SkeletonComponent();

	void CreatePose(size_t BoneStartIndex, size_t BonesCount, std::vector<glm::mat4> * Destination) const;

	// ISkeletonComponent3D
	std::shared_ptr<CM2SkeletonBone> GetBone(size_t Index) const;

	// CSceneNodeComponentBase
	void Update(const UpdateEventArgs& e) override;

protected:
	const CM2_Base_Instance& GetM2OwnerNode() const;

private:
	std::vector<std::shared_ptr<CM2SkeletonBone>> m_Bones;
};
