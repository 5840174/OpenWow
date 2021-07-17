#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Part_Color.h"

CM2_Part_Color::CM2_Part_Color(const CM2& M2Object, const std::shared_ptr<IByteBuffer>& File, const SM2_Color& M2Color)
	: m_M2Object(M2Object)
{
	m_ColorAnimated.Initialize(M2Color.color, File, M2Object.getSkeleton().GetAnimFiles());
	m_AlphaAnimated.Initialize(M2Color.alpha, File, M2Object.getSkeleton().GetAnimFiles());
}

CM2_Part_Color::~CM2_Part_Color()
{
}

glm::vec4 CM2_Part_Color::GetColorAndAlpha(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const
{
	_ASSERT(M2Instance != nullptr);

	glm::vec4 colorAndAlpha = glm::vec4(1.0f, 0.0f, 0.0f, 0.25f);

	if (const auto& animator = M2Instance->GetAnimatorComponent())
	{
		if (m_ColorAnimated.IsUsesBySequence(animator->getSequenceIndex()))
			colorAndAlpha.xyz = m_ColorAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);

		if (m_AlphaAnimated.IsUsesBySequence(animator->getSequenceIndex()))
			colorAndAlpha.a = m_AlphaAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);
	}

	return colorAndAlpha;
}

glm::vec3 CM2_Part_Color::GetColor(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const
{
	_ASSERT(M2Instance != nullptr);

	if (const auto& animator = M2Instance->GetAnimatorComponent())
		if (m_ColorAnimated.IsUsesBySequence(animator->getSequenceIndex()))
			return m_ColorAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);
	return glm::vec3(1.0f, 0.0f, 0.0f);
}

float CM2_Part_Color::GetAlpha(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const
{
	_ASSERT(M2Instance != nullptr);

	if (const auto& animator = M2Instance->GetAnimatorComponent())
		if (m_AlphaAnimated.IsUsesBySequence(animator->getSequenceIndex()))
			return m_AlphaAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);
	return 0.5f;
}
