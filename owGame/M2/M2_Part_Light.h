#pragma once

#ifdef USE_M2_MODELS

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

class SM2_Part_Light_Wrapper
{
public:
	SM2_Part_Light_Wrapper(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Light& M2Light);
	virtual ~SM2_Part_Light_Wrapper();

	CLight    GetLight(const CM2_Base_Instance* M2Instance, uint32 globalTime) const;
	int16     GetAttachBone() const { return m_M2Light.bone; }
	glm::vec3 GetPosition() const { return Fix_XZmY(m_M2Light.position); }

private:
	M2_Animated<glm::vec3>	ambColor;
	M2_Animated<float>		ambIntensity;
	M2_Animated<glm::vec3>	diffColor;
	M2_Animated<float>		diffIntensity;
	M2_Animated<float>		attenuation_start;
	M2_Animated<float>		attenuation_end;
	M2_Animated<uint8>		visibility;

private:
	const CM2& m_M2Object;
	const SM2_Light& m_M2Light;
};

#endif
