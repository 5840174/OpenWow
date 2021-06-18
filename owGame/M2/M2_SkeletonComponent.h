#pragma once

#include "M2/M2_Part_Bone.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END


//
// CM2SkeletonBone3D
//
class ZN_API CM2SkeletonBone3D
	: public std::enable_shared_from_this<CM2SkeletonBone3D>
{
public:
	CM2SkeletonBone3D(const SM2_Part_Bone_Wrapper& M2Bone);
	virtual ~CM2SkeletonBone3D();

	// ISkeletonComponentBone3D
	const std::weak_ptr<CM2SkeletonBone3D>& GetParentBone() const;
	const std::vector<std::shared_ptr<CM2SkeletonBone3D>>& GetChilds() const;
	glm::vec3 GetPivotPoint() const;
	const glm::mat4& GetMatrix() const;
	const glm::mat4& GetRotateMatrix() const;

	// Internal
	void SetParentAndChildsInternals(const std::vector<std::shared_ptr<CM2SkeletonBone3D>>& Bones);
	void Calculate(const CM2_Base_Instance* M2Instance, const ICameraComponent3D* Camera, uint32 GlobalTime);
	void Reset();

private:
	const SM2_Part_Bone_Wrapper&                   m_M2Bone;
	std::weak_ptr<CM2SkeletonBone3D>               m_ParentBone;
	std::vector<std::shared_ptr<CM2SkeletonBone3D>>m_Childs;
	glm::vec3                                      m_PivotPoint;
	glm::mat4                                      m_Matrix;
	glm::mat4                                      m_RotateMatrix;
	bool                                           m_IsCalculated;
};



//
// CM2SkeletonComponent3D
//
class ZN_API CM2SkeletonComponent3D
	: public CComponentBase
{
public:
	ZN_OBJECTCLASS(UINT16_MAX - 600u);

	CM2SkeletonComponent3D(const CM2_Base_Instance& OwnerNode);
	virtual ~CM2SkeletonComponent3D();

	std::vector<glm::mat4> CreatePose(size_t BoneStartIndex, size_t BonesCount) const;

	// ISkeletonComponent3D
	std::shared_ptr<CM2SkeletonBone3D> GetBone(size_t Index) const;

	// CComponentBase
	void Update(const UpdateEventArgs& e) override;

protected:
	const CM2_Base_Instance* GetM2OwnerNode() const;

private:
	std::vector<std::shared_ptr<CM2SkeletonBone3D>> m_Bones;
};
