#include "stdafx.h"

// Include
#include "M2_Comp_Skeleton.h"

// General
#include "M2_Part_Bone.h"

CM2_Part_Bone::CM2_Part_Bone(std::shared_ptr<IFile> f, const SM2_Bone& _proto, cGlobalLoopSeq global)
{
	m_GameBoneId = _proto.key_bone_id;
	m_Flags = _proto.flags;

	m_ParentBoneID = _proto.parent_bone;
	submesh = _proto.submesh_id;

	trans.init(_proto.translation, f, global, Fix_XZmY);
	roll.init(_proto.rotation, f, global, Fix_XZmYW);
	scale.init(_proto.scale, f, global, Fix_XZY);

	pivot = Fix_XZmY(_proto.pivot);
}

void CM2_Part_Bone::setParentBone(std::shared_ptr<CM2_Comp_Skeleton> _skeleton)
{
	if (m_ParentBoneID != -1)
	{
		m_ParentBone = _skeleton->getBoneDirect(m_ParentBoneID);
	}
}

void CM2_Part_Bone::calcMatrix(uint16 anim, uint32 time, uint32 globalTime)
{
	if (m_IsCalculated)
	{
		return;
	}

	std::shared_ptr<CM2_Part_Bone> ParentBone = m_ParentBone.lock();
	if (ParentBone != nullptr)
	{
		ParentBone->calcMatrix(anim, time, globalTime);
	}

	mat4 m(1.0f);
	if (IsInterpolated(anim))
	{
		m = glm::translate(m, pivot);

		if (trans.uses(anim))
		{
			m = glm::translate(m, trans.getValue(anim, time, globalTime));
		}

		if (roll.uses(anim))
		{
			quat q = roll.getValue(anim, time, globalTime);
			m *= glm::toMat4(q);

			if (ParentBone != nullptr)
			{
				_ASSERT(ParentBone->IsCalculated());
				m_RotationMatrix = ParentBone->m_RotationMatrix * glm::toMat4(q);
			}
			else
			{
				m_RotationMatrix = glm::toMat4(q);
			}
		}

		if (scale.uses(anim))
		{
			m = glm::scale(m, scale.getValue(anim, time, globalTime));
		}

		m = glm::translate(m, pivot * -1.0f);
	}

	if (ParentBone != nullptr)
	{
		_ASSERT(ParentBone->IsCalculated());
		m_TransformMatrix = ParentBone->getTransformMatrix() * m;
	}
	else
	{
		m_TransformMatrix = m;
	}

	transPivot = m_TransformMatrix * vec4(pivot, 0);

	m_IsCalculated = true;
}

void CM2_Part_Bone::calcBillboard(cmat4 _viewMatrix, cmat4 _worldMatrix)
{
	if (IsBillboard())
	{
		m_TransformMatrix = glm::translate(m_TransformMatrix, pivot);
		{
			mat4 W = _worldMatrix * m_TransformMatrix;
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

			m_TransformMatrix[0][0] = vForward.x;
			m_TransformMatrix[0][1] = vForward.y;
			m_TransformMatrix[0][2] = vForward.z;

			m_TransformMatrix[1][0] = vUp.x;
			m_TransformMatrix[1][1] = vUp.y;
			m_TransformMatrix[1][2] = vUp.z;

			m_TransformMatrix[2][0] = vRight.x;
			m_TransformMatrix[2][1] = vRight.y;
			m_TransformMatrix[2][2] = vRight.z;
		}
		m_TransformMatrix = glm::translate(m_TransformMatrix, pivot * -1.0f);
	}
}
