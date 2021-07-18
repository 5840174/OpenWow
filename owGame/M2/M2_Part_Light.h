#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

class ZN_API CM2_Part_Light
{
public:
	CM2_Part_Light(const CM2& M2Object, const std::shared_ptr<IByteBuffer>& ByteBuffer, const SM2_Light& M2Light);
	virtual ~CM2_Part_Light();

	int16                   GetAttachBone() const { return m_M2Light.bone; }
	glm::vec3               GetPosition() const { return Fix_From_XZmY_To_XYZ(m_M2Light.position); }

	const SM2_Light&        GetProto() const { return m_M2Light; }

public:
	M2_Animated<ColorRGB>  ambColor;
	M2_Animated<float>	   ambIntensity;

	M2_Animated<ColorRGB>  diffColor;
	M2_Animated<float>	   diffIntensity;

	M2_Animated<float>	   attenuation_start;
	M2_Animated<float>	   attenuation_end;

	M2_Animated<uint8>	   visibility;

private:
	const CM2& m_M2Object;
	const SM2_Light m_M2Light;
};
