#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_Color.h"

CM2_Part_Color::CM2_Part_Color(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Color& M2Color)
	: m_M2Object(M2Object)
{
	m_ColorAnimated.Initialize(M2Color.color, File);
	m_AlphaAnimated.Initialize(M2Color.alpha, File);
}

CM2_Part_Color::~CM2_Part_Color()
{
}

glm::vec4 CM2_Part_Color::GetColorAndAlpha(uint16 Sequence, uint32 Time, uint32 GlobalTime) const
{
	glm::vec4 colorAndAlpha = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	if (m_ColorAnimated.IsUsesBySequence(Sequence))
		colorAndAlpha.xyz = m_ColorAnimated.GetValue(Sequence, Time, m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);

	if (m_AlphaAnimated.IsUsesBySequence(Sequence))
		colorAndAlpha.a = m_AlphaAnimated.GetValue(Sequence, Time, m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);

	return colorAndAlpha;
}

glm::vec3 CM2_Part_Color::GetColor(uint16 Sequence, uint32 Time, uint32 GlobalTime) const
{
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	if (m_ColorAnimated.IsUsesBySequence(Sequence))
		color = m_ColorAnimated.GetValue(Sequence, Time, m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);
	return color;
}

float CM2_Part_Color::GetAlpha(uint16 Sequence, uint32 Time, uint32 GlobalTime) const
{
	float alpha = 1.0f;
	if (m_AlphaAnimated.IsUsesBySequence(Sequence))
		alpha = m_AlphaAnimated.GetValue(Sequence, Time, m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);
	return alpha;
}
