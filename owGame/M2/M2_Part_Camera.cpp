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
	
	m_PositionBase = Fix_From_XZmY_To_XYZ(M2Camera.position_base);
	m_TargetBase   = Fix_From_XZmY_To_XYZ(M2Camera.target_position_base);

	m_Positions.Initialize(M2Camera.positions, ByteBuffer, M2Object.getSkeleton().GetAnimFiles(), Fix_From_XZmY_To_XYZ);
	m_Targets.Initialize(M2Camera.target_position, ByteBuffer, M2Object.getSkeleton().GetAnimFiles(), Fix_From_XZmY_To_XYZ);
	m_Rolls.Initialize(M2Camera.roll, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());
}

CM2_Part_Camera::~CM2_Part_Camera()
{
}

bool CM2_Part_Camera::Calculate(const CM2_Base_Instance& M2Instance, uint32 globalTime, float AspectRatio, const glm::mat4& OriginMatrix, glm::vec3 * Position, glm::vec3 * Direction, glm::mat4 * Projection, glm::mat4 * View) const
{
	const auto& animator = M2Instance.GetAnimatorComponent();
	if (animator == nullptr)
		return false;

	glm::vec3 position = m_PositionBase;
	if (m_Positions.IsUsesBySequence(animator->getSequenceIndex()))
		position += m_Positions.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime, true);
	position = OriginMatrix * glm::vec4(position, 1.0f);
	*Position = position;

	glm::vec3 target = m_TargetBase;
	if (m_Targets.IsUsesBySequence(animator->getSequenceIndex()))
		target += m_Targets.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime, true);
	target = OriginMatrix * glm::vec4(target, 1.0f);

	const glm::vec3 direction = glm::normalize(target - position);
	*Direction = direction;

	float roll = 0.0f;
	if (m_Rolls.IsUsesBySequence(animator->getSequenceIndex()))
		roll = m_Rolls.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

	float fovRadians = m_DFov / glm::sqrt(1.0f + glm::pow(AspectRatio, 2.0f));

	glm::vec3 upVector(0.0f, 1.0f, 0.0f);

	// Bug with roll
	//glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), roll, direction);
	//upVector = rollMatrix * glm::vec4(upVector, 0.0f);

	*Projection = glm::perspectiveRH_ZO(fovRadians, AspectRatio, m_NearClip * 10.0f, m_FarClip * 100);
	*View = glm::lookAt(position, position + direction, upVector);

	return true;
}
