#pragma once

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

#include "M2_Part_Bone.h"

class ZN_API CM2SkeletonBone
	: public std::enable_shared_from_this<CM2SkeletonBone>
{
public:
	CM2SkeletonBone(const CM2_Part_Bone& M2Bone);
	virtual ~CM2SkeletonBone();

	// ISkeletonComponentBone3D
	const std::weak_ptr<CM2SkeletonBone>& GetParentBone() const;
	const std::vector<std::shared_ptr<CM2SkeletonBone>>& GetChilds() const;
	glm::vec3 GetPivotPoint() const;
	const glm::mat4& GetMatrix() const;
	const glm::mat4& GetRotateMatrix() const;

	// Internal
	void SetParentAndChildsInternals(const std::vector<std::shared_ptr<CM2SkeletonBone>>& Bones);
	void Calculate(const CM2_Base_Instance& M2Instance, const ICameraComponent3D* Camera, uint32 GlobalTime);
	void Reset();

private:
	const CM2_Part_Bone&                   m_M2Bone;
	std::weak_ptr<CM2SkeletonBone>               m_ParentBone;
	std::vector<std::shared_ptr<CM2SkeletonBone>>m_Childs;
	glm::vec3                                      m_PivotPoint;
	glm::mat4                                      m_Matrix;
	glm::mat4                                      m_RotateMatrix;
	bool                                           m_IsCalculated;
};
