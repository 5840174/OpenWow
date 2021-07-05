#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Part_Camera.h"

CM2_Part_Camera::CM2_Part_Camera(const CM2& M2Object, const std::shared_ptr<IByteBuffer>& ByteBuffer, const SM2_Camera& M2Camera)
	: m_M2Object(M2Object)
{
	m_DFov = M2Camera.fov;
	m_FarClip = M2Camera.far_clip;
	m_NearClip = M2Camera.near_clip;
	
	m_PositionBase = Fix_XZmY(M2Camera.position_base);
	m_TargetBase = Fix_XZmY(M2Camera.target_position_base);

	m_Positions.Initialize(M2Camera.positions, ByteBuffer, M2Object.getSkeleton().GetAnimFiles(), Fix_XZmY);
	m_Targets.Initialize(M2Camera.target_position, ByteBuffer, M2Object.getSkeleton().GetAnimFiles(), Fix_XZmY);
	m_Rolls.Initialize(M2Camera.roll, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());
}

CM2_Part_Camera::~CM2_Part_Camera()
{
}

bool CM2_Part_Camera::Calculate(const CM2_Base_Instance * M2Instance, uint32 globalTime, float AspectRatio, glm::mat4 * Projection, glm::mat4 * View) const
{
	const auto& animator = M2Instance->getAnimator();
	if (animator == nullptr)
		return false;

	glm::vec3 position = m_PositionBase;
	if (m_Positions.IsUsesBySequence(animator->getSequenceIndex()))
		position += m_Positions.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

	glm::vec3 target = m_TargetBase;
	if (m_Targets.IsUsesBySequence(animator->getSequenceIndex()))
		target += m_Targets.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

	float roll = 0.0f;
	if (m_Rolls.IsUsesBySequence(0))
		roll = m_Rolls.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

	float fov = m_DFov / glm::sqrt(1.0f + glm::pow(AspectRatio, 2.0f));

	*Projection = glm::perspectiveRH_ZO(fov, AspectRatio, m_NearClip, m_FarClip);
	*View = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));

	return true;
}
