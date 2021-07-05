#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

class ZN_API CM2_Part_Camera
{
public:
	CM2_Part_Camera(const CM2& M2Model, const std::shared_ptr<IByteBuffer>& File, const SM2_Camera& M2Camera);
	virtual ~CM2_Part_Camera();

	bool Calculate(const CM2_Base_Instance* M2Instance, uint32 globalTime, float AspectRatio, glm::mat4 * Projection, glm::mat4 * View) const;

private:
	float m_DFov;

	float m_FarClip;
	float m_NearClip;

	glm::vec3 m_PositionBase;
	M2_Animated<glm::vec3> m_Positions;
	
	glm::vec3 m_TargetBase;
	M2_Animated<glm::vec3> m_Targets;
	
	M2_Animated<float> m_Rolls;
	float rollResult;

private:
	const CM2& m_M2Object;
};
