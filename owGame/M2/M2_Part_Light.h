#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class M2;
// FORWARD END

class CM2_Part_Light
{
public:
	CM2_Part_Light(const std::weak_ptr<M2> _parentM2, std::shared_ptr<IFile> f, const SM2_Light& _proto, cGlobalLoopSeq global);

	void setup(uint16 anim, uint32 time, uint32 globalTime);

private:
	SM2_Light::Type			type;
	std::weak_ptr<const CM2_Part_Bone>	m_Bone;
	vec3					position;

	M2_Animated<vec3>		ambColor;
	M2_Animated<float>		ambIntensity;
	M2_Animated<vec3>		diffColor;
	M2_Animated<float>		diffIntensity;
	M2_Animated<float>		attenuation_start;
	M2_Animated<float>		attenuation_end;
	M2_Animated<uint8>		visibility;

	// Values
	vec3					positionValue;
	vec3					directionValue;
	vec3					ambColorValue;
	float					ambIntensityValue;
	vec3					diffColorValue;
	float					diffIntensityValue;
	float					attenuation_startValue;
	float					attenuation_endValue;
	uint8					visibilityValue;

private:
	const vec3 direction = vec3(0.0f, 1.0f, 0.0f);
};