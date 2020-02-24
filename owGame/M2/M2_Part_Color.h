#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class M2;
// FORWARD END

class CM2_Part_Color
{
public:
	CM2_Part_Color(const M2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Color& M2Color);
	virtual ~CM2_Part_Color();

	glm::vec4 GetColorAndAlpha(uint16 Sequence, uint32 Time, uint32 GlobalTime) const;
	glm::vec3 GetColor(uint16 Sequence, uint32 Time, uint32 GlobalTime) const;
	float     GetAlpha(uint16 Sequence, uint32 Time, uint32 GlobalTime) const;

private:
	M2_Animated<vec3> m_ColorAnimated;
	M2_Animated<float, short, ShortToFloat> m_AlphaAnimated;

	const M2& m_M2Object;
};