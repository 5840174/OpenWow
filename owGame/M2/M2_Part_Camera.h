#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
// FORWARD END

class CM2_Part_Camera
{
public:
	CM2_Part_Camera(const CM2& M2Model, const std::shared_ptr<IFile>& File, const SM2_Camera& M2Camera);
	virtual ~CM2_Part_Camera();

	vec3 GetTranslation() const { return pResult; }
	vec3 getTarget() const { return tResult; }

	void calc(uint32 time, uint32 globalTime);
	void setup(cvec3 _startPoint, float rotate);
	void getParams(vec3* _position, vec3* _target, float* _fov, float* _nearPlane, float* _farPlane);

private:
	float farclip;
	float nearclip;

	M2_Animated<vec3> tPos;
	vec3 m_PositionBase;
	vec3 pResult;

	M2_Animated<vec3> tTarget;
	vec3 m_TargetBase;
	vec3 tResult;

	M2_Animated<float> tRoll;
	float rollResult;

	float fov;

private:
	const CM2& m_M2Object;
};