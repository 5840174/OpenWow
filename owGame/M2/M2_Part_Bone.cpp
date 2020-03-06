#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Comp_Skeleton.h"

// General
#include "M2_Part_Bone.h"

CM2_Part_Bone::CM2_Part_Bone(const CM2& M2Object, const std::shared_ptr<IFile>& File, int16 boneDirectIndex, const SM2_Bone& M2Bone)
	: m_M2Object(M2Object)
	, m_DirectIndex(boneDirectIndex)
{
	m_GameBoneId = M2Bone.key_bone_id;
	if (M2Bone.key_bone_id != -1)
	{
		_ASSERT(M2Bone.key_bone_id >= 0 && M2Bone.key_bone_id < 27);
		m_GameBoneName = M2_GameBoneTypeNames[M2Bone.key_bone_id];
	}
	m_Flags = M2Bone.flags;
	m_ParentBoneID = M2Bone.parent_bone;
	m_submesh = M2Bone.submesh_id;
	m_PivotPoint = Fix_XZmY(M2Bone.pivot);
	m_TranslateAnimated.Initialize(M2Bone.translation, File, Fix_XZmY);
	m_RotateAnimated.Initialize(M2Bone.rotation, File, Fix_XZmYW);
	m_ScaleAnimated.Initialize(M2Bone.scale, File, Fix_XZY);
}


CM2_Part_Bone::~CM2_Part_Bone()
{
}

glm::mat4 CM2_Part_Bone::calcMatrix(uint16 anim, uint32 time, uint32 globalTime) const
{
	/*std::shared_ptr<const CM2_Part_Bone> ParentBone = m_ParentBone.lock();
	if (ParentBone != nullptr)
	{
		ParentBone->calcMatrix(anim, time,  globalTime);
	}*/

	glm::mat4 m(1.0f);
	if (IsInterpolated(anim))
	{
		m = glm::translate(m, m_PivotPoint);

		if (m_TranslateAnimated.IsUsesBySequence(anim))
		{
			m = glm::translate(m, m_TranslateAnimated.GetValue(anim, time, m_M2Object.getSkeleton().getGlobalLoops(), globalTime));
		}

		if (m_RotateAnimated.IsUsesBySequence(anim))
		{
			glm::quat q = m_RotateAnimated.GetValue(anim, time, m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
			m *= glm::toMat4(q);

			/*if (ParentBone != nullptr)
			{
				_ASSERT(ParentBone->IsCalculated());
				m_RotationMatrix = ParentBone->m_RotationMatrix * glm::toMat4(q);
			}
			else
			{
				m_RotationMatrix = glm::toMat4(q);
			}*/
		}

		if (m_ScaleAnimated.IsUsesBySequence(anim))
		{
			m = glm::scale(m, m_ScaleAnimated.GetValue(anim, time, m_M2Object.getSkeleton().getGlobalLoops(), globalTime));
		}

		m = glm::translate(m, m_PivotPoint * -1.0f);
	}

	return m;

	/*if (ParentBone != nullptr)
	{
		_ASSERT(ParentBone->IsCalculated());
		m_TransformMatrix = ParentBone->getTransformMatrix() * m;
	}
	else
	{
		m_TransformMatrix = m;
	}*/
}

glm::mat4 CM2_Part_Bone::calcBillboardMatrix(cmat4 _viewMatrix, cmat4 _worldMatrix) const
{
	glm::mat4 m(1.0f);
	if (IsBillboard())
	{
		m = glm::translate(m, m_PivotPoint);
		{
			mat4 W = _worldMatrix * m;
			mat4 VW = _viewMatrix * W;

			// Set vectors default
			vec3 worldScale = extractScale(W);
			vec3 vRight   = vec3(VW[0][0], VW[1][0], VW[2][0]) / worldScale.x;
			vec3 vUp      = vec3(VW[0][1], VW[1][1], VW[2][1]) / worldScale.y;
			vec3 vForward = vec3(VW[0][2], VW[1][2], VW[2][2]) / worldScale.z;
			vRight *= -1.0f;

			if (m_Flags.cylindrical_billboard_lock_x)
			{
				vUp = vec3(VW[0][1], 0, 0);
			}
			else if (m_Flags.cylindrical_billboard_lock_y)
			{
				vUp = vec3(0, VW[1][1], 0);
			}
			else if (m_Flags.cylindrical_billboard_lock_z)
			{
				vUp = vec3(0, 0, VW[2][1]);
			}

			m[0][0] = vForward.x;
			m[0][1] = vForward.y;
			m[0][2] = vForward.z;

			m[1][0] = vUp.x;
			m[1][1] = vUp.y;
			m[1][2] = vUp.z;

			m[2][0] = vRight.x;
			m[2][1] = vRight.y;
			m[2][2] = vRight.z;
		}
		m = glm::translate(m, m_PivotPoint * -1.0f);
	}

	return m;
}



void CM2_Part_Bone::Print(size_t intent) const
{
	std::string message = "";
	for (size_t i = 0; i < intent; i++) message += "---";
	if (IsInterpolated(0))
		Log::Print("%s%s (%d) (%s)", message.c_str(), ((m_GameBoneName.empty() == false) ? m_GameBoneName.c_str() : "???"), m_DirectIndex, IsInterpolated(0) ? " " : " ");
	for (const auto& ch : m_ChildBones) ch->Print(intent + 1);
}

//
// Call this after initialization
//
void CM2_Part_Bone::SetParentBoneInternal(const CM2_Comp_Skeleton& Skeleton)
{
	if (m_ParentBoneID != -1)
	{
		auto parentBone = Skeleton.getBoneDirect(m_ParentBoneID);
		std::const_pointer_cast<CM2_Part_Bone>(parentBone)->m_ChildBones.push_back(shared_from_this());
		m_ParentBone = parentBone;
	}
}
