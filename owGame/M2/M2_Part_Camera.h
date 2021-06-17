#pragma once

#ifdef USE_M2_MODELS

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
// FORWARD END

class SM2_Part_Camera_Wrapper
{
public:
	SM2_Part_Camera_Wrapper(const CM2& M2Model, const std::shared_ptr<IFile>& File, const SM2_Camera& M2Camera);
	virtual ~SM2_Part_Camera_Wrapper();

	glm::vec3 GetTranslation() const { return pResult; }
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

#endif
