#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
// FORWARD END

class CM2_Part_Camera
{
public:
	CM2_Part_Camera(const CM2& M2Model, const std::shared_ptr<IByteBuffer>& File, const SM2_Camera& M2Camera);
	virtual ~CM2_Part_Camera();

	glm::vec3 GetPosition() const { return pResult; }
	glm::vec3 getTarget() const { return tResult; }

	void calc(uint32 time, uint32 globalTime);
	void setup(const glm::vec3& _startPoint, float rotate);
	void getParams(glm::vec3* _position, glm::vec3* _target, float* _fov, float* _nearPlane, float* _farPlane);

private:
	float farclip;
	float nearclip;

	M2_Animated<glm::vec3> tPos;
	glm::vec3 m_PositionBase;
	glm::vec3 pResult;

	M2_Animated<glm::vec3> tTarget;
	glm::vec3 m_TargetBase;
	glm::vec3 tResult;

	M2_Animated<float> tRoll;
	float rollResult;

	float fov;

private:
	const CM2& m_M2Object;
};
