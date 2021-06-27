#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Part_Bone.h"

SM2_Part_Bone_Wrapper::SM2_Part_Bone_Wrapper(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Bone& M2Bone)
	: m_M2Object(M2Object)
	, m_M2Bone(M2Bone)
{
	m_TranslateAnimated.Initialize(M2Bone.translation, File, M2Object.getSkeleton().GetAnimFiles(), Fix_XZmY);
	m_RotateAnimated.Initialize(M2Bone.rotation, File, M2Object.getSkeleton().GetAnimFiles(), Fix_XZmYW);
	m_ScaleAnimated.Initialize(M2Bone.scale, File, M2Object.getSkeleton().GetAnimFiles(), Fix_XZY);
}


SM2_Part_Bone_Wrapper::~SM2_Part_Bone_Wrapper()
{
}

glm::mat4 SM2_Part_Bone_Wrapper::calcMatrix(const CM2_Base_Instance* M2Instance, uint32 globalTime) const
{
	glm::mat4 m(1.0f);
	if (const auto& animator = M2Instance->getAnimator())
	{
		if (IsInterpolated(animator->getSequenceIndex()))
		{
			m = glm::translate(m, getPivot());

			if (m_TranslateAnimated.IsUsesBySequence(animator->getSequenceIndex()))
			{
				m = glm::translate(m, m_TranslateAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime));
			}

			if (m_RotateAnimated.IsUsesBySequence(animator->getSequenceIndex()))
			{
				m *= glm::toMat4(m_RotateAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime));
			}

			if (m_ScaleAnimated.IsUsesBySequence(animator->getSequenceIndex()))
			{
				m = glm::scale(m, m_ScaleAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime));
			}

			m = glm::translate(m, getPivot() * -1.0f);
		}
	}

	return m;
}

glm::mat4 SM2_Part_Bone_Wrapper::calcRotationMatrix(const CM2_Base_Instance * M2Instance, uint32 globalTime) const
{
	if (const auto& animator = M2Instance->getAnimator())
	{
		if (m_RotateAnimated.IsUsesBySequence(animator->getSequenceIndex()))
		{
			return glm::toMat4(m_RotateAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime));
		}
	}

	return glm::mat4(1.0f);
}

glm::mat4 SM2_Part_Bone_Wrapper::calcBillboardMatrix(const glm::mat4& CalculatedMatrix, const CM2_Base_Instance* M2Instance, const ICameraComponent3D* Camera) const
{
	glm::mat4 m(1.0f);
	_ASSERT(IsBillboard());

	m = glm::translate(m, getPivot());
	{
		glm::mat4 W = m;
		W = CalculatedMatrix * W;
		W = M2Instance->GetWorldTransfom() * W;

		glm::mat4 VW = Camera->GetViewMatrix() * W;

		// Set vectors default
		glm::vec3 worldScale = extractScale(W);
		glm::vec3 vRight     = glm::vec3(VW[0][0], VW[1][0], VW[2][0]) / worldScale.x;
		glm::vec3 vUp        = glm::vec3(VW[0][1], VW[1][1], VW[2][1]) / worldScale.y;
		glm::vec3 vForward   = glm::vec3(VW[0][2], VW[1][2], VW[2][2]) / worldScale.z;
		vRight *= -1.0f;



		if (m_M2Bone.flags.cylindrical_billboard_lock_x) // TODO: investigate it
		{
			vUp = glm::vec3(VW[0][1], 0, 0) / worldScale.x;
		}

		else if (m_M2Bone.flags.cylindrical_billboard_lock_y)
		{
			vUp = glm::vec3(0, VW[1][1], 0) / worldScale.y;
		}

		else if (m_M2Bone.flags.cylindrical_billboard_lock_z)
		{
			vUp = glm::vec3(0, 0, VW[2][1]) / worldScale.z;
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
	m = glm::translate(m, getPivot() * -1.0f);

	return m;
}
