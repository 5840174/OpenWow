#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_SkeletonBone.h"


CM2SkeletonBone::CM2SkeletonBone(const CM2_Part_Bone& M2Bone)
	: m_M2Bone(M2Bone)
	, m_PivotPoint(glm::vec3(0.0f))
	, m_Matrix(glm::mat4(1.0f))
	, m_RotateMatrix(glm::mat4(1.0f))
	, m_IsCalculated(false)
{}

CM2SkeletonBone::~CM2SkeletonBone()
{}

const std::weak_ptr<CM2SkeletonBone>& CM2SkeletonBone::GetParentBone() const
{
	return m_ParentBone;
}

const std::vector<std::shared_ptr<CM2SkeletonBone>>& CM2SkeletonBone::GetChilds() const
{
	return m_Childs;
}

glm::vec3 CM2SkeletonBone::GetPivotPoint() const
{
	return m_M2Bone.getPivot();
}

const glm::mat4& CM2SkeletonBone::GetMatrix() const
{
	return m_Matrix;
}

const glm::mat4& CM2SkeletonBone::GetRotateMatrix() const
{
	return m_RotateMatrix;
}

//
// Internal
//
void CM2SkeletonBone::SetParentAndChildsInternals(const std::vector<std::shared_ptr<CM2SkeletonBone>>& Bones)
{
	if (m_M2Bone.getParentBoneID() != -1)
	{
		_ASSERT(m_M2Bone.getParentBoneID() < Bones.size());
		auto parentBone = Bones[m_M2Bone.getParentBoneID()];
		_ASSERT(std::find(parentBone->m_Childs.begin(), parentBone->m_Childs.end(), shared_from_this()) == parentBone->m_Childs.end());
		parentBone->m_Childs.push_back(shared_from_this());
		m_ParentBone = parentBone;
	}
}

void CM2SkeletonBone::Calculate(const CM2_Base_Instance& M2Instance, const ICameraComponent3D* Camera, uint32 GlobalTime)
{
	if (m_IsCalculated)
		return;

	auto parentBone = m_ParentBone.lock();
	if (parentBone)
		std::dynamic_pointer_cast<CM2SkeletonBone>(parentBone)->Calculate(M2Instance, Camera, GlobalTime);

	m_Matrix = m_M2Bone.calcMatrix(M2Instance, GlobalTime);
	m_RotateMatrix = m_M2Bone.calcRotationMatrix(M2Instance, GlobalTime);

	if (parentBone)
	{
		m_Matrix = parentBone->GetMatrix() * m_Matrix;
		m_RotateMatrix = parentBone->GetRotateMatrix() * m_RotateMatrix;
	}

	//if (m_M2Bone.IsBillboard())
	//{
	//	m_Matrix = m_Matrix * m_M2Bone.calcBillboardMatrix(m_Matrix, M2Instance, Camera);
	//}

	m_IsCalculated = true;
}

void CM2SkeletonBone::Reset()
{
	m_IsCalculated = false;
}
