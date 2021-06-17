#pragma once

#ifdef USE_M2_MODELS

class CM2;
class CM2_Base_Instance;
#include "M2/M2_Part_Bone.h"


//
// CM2SkeletonBone3D
//
class ZN_API CM2SkeletonBone3D
	: public ISkeletonBone
	, public std::enable_shared_from_this<CM2SkeletonBone3D>
{
public:
	CM2SkeletonBone3D(const SM2_Part_Bone_Wrapper& M2Bone);
	virtual ~CM2SkeletonBone3D();

	// ISkeletonComponentBone3D
	const std::weak_ptr<ISkeletonBone>& GetParentBone() const override;
	const std::vector<std::shared_ptr<ISkeletonBone>>& GetChilds() const override;
	glm::vec3 GetPivotPoint() const override;
	const glm::mat4& GetMatrix() const override;
	const glm::mat4& GetRotateMatrix() const override;

	// Internal
	void SetParentAndChildsInternals(const std::vector<std::shared_ptr<CM2SkeletonBone3D>>& Bones);
	void Calculate(const CM2_Base_Instance* M2Instance, const ICameraComponent3D* Camera, uint32 GlobalTime);
	void Reset();

private:
	const SM2_Part_Bone_Wrapper&                   m_M2Bone;
	std::weak_ptr<ISkeletonBone>                 m_ParentBone;
	std::vector<std::shared_ptr<ISkeletonBone>>  m_Childs;
	glm::vec3                                      m_PivotPoint;
	glm::mat4                                      m_Matrix;
	glm::mat4                                      m_RotateMatrix;
	bool                                           m_IsCalculated;
};



//
// CM2SkeletonComponent3D
//
class ZN_API CM2SkeletonComponent3D
	: public ISkeletonComponent3D
	, public CComponentBase
{
public:
	CM2SkeletonComponent3D(const CM2_Base_Instance& OwnerNode);
	virtual ~CM2SkeletonComponent3D();

	std::vector<glm::mat4> CreatePose(size_t BoneStartIndex, size_t BonesCount) const;

	// ISkeletonComponent3D
	std::shared_ptr<ISkeletonComponentBone3D> GetBone(size_t Index) const override;

	// CComponentBase
	void Update(const UpdateEventArgs& e) override;

protected:
	const CM2_Base_Instance& GetM2OwnerNode() const;

private:
	std::vector<std::shared_ptr<CM2SkeletonBone3D>> m_Bones;
};

#endif