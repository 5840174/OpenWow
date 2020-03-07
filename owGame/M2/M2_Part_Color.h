#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

class CM2_Part_Color
{
public:
	CM2_Part_Color(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Color& M2Color);
	virtual ~CM2_Part_Color();

	glm::vec4 GetColorAndAlpha(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const;
	glm::vec3 GetColor(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const;
	float     GetAlpha(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const;

private:
	M2_Animated<vec3> m_ColorAnimated;
	M2_Animated<float, short, ShortToFloat> m_AlphaAnimated;

	const CM2& m_M2Object;
};