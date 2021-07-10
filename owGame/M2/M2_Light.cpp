#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Light.h"

CM2Light::CM2Light(const IBaseManager& BaseManager, const CM2_Part_Light & M2Light)
	: CLight(BaseManager)
	, m_M2Light(M2Light)
{}

CM2Light::~CM2Light()
{}

void CM2Light::Calculate(const CM2_Base_Instance& M2Instance, uint32 GlobalTime)
{
	if (m_M2Light.GetProto().type == SM2_Light::Type::Directional)
	{
		SetType(ELightType::Directional);
	}
	else if (m_M2Light.GetProto().type == SM2_Light::Type::Point)
	{
		SetType(ELightType::Point);
	}
	else
		throw CException("CM2Light::Calculate: Unknown light type.");


	if (const auto& animator = M2Instance.getAnimator())
	{
		// Visibility
		if (m_M2Light.visibility.IsUsesBySequence(animator->getSequenceIndex()))
			SetEnabled(m_M2Light.visibility.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), M2Instance.GetM2().getSkeleton().getGlobalLoops(), GlobalTime));

		if (m_M2Light.ambColor.IsUsesBySequence(animator->getSequenceIndex()))
			SetAmbientColor(m_M2Light.ambColor.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), M2Instance.GetM2().getSkeleton().getGlobalLoops(), GlobalTime));
		//if (ambIntensity.IsUsesBySequence(animator->getSequenceIndex()))
		//	ambient.a = ambIntensity.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

		if (m_M2Light.diffColor.IsUsesBySequence(animator->getSequenceIndex()))
			SetColor(m_M2Light.diffColor.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), M2Instance.GetM2().getSkeleton().getGlobalLoops(), GlobalTime));

		if (m_M2Light.diffIntensity.IsUsesBySequence(animator->getSequenceIndex()))
			SetIntensity(m_M2Light.diffIntensity.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), M2Instance.GetM2().getSkeleton().getGlobalLoops(), GlobalTime));

		float attenStart = 0.0f;
		if (m_M2Light.attenuation_start.IsUsesBySequence(animator->getSequenceIndex()))
			attenStart = m_M2Light.attenuation_start.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), M2Instance.GetM2().getSkeleton().getGlobalLoops(), GlobalTime);

		float attenEnd = 0.0f;
		if (m_M2Light.attenuation_end.IsUsesBySequence(animator->getSequenceIndex()))
			attenEnd = m_M2Light.attenuation_end.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), M2Instance.GetM2().getSkeleton().getGlobalLoops(), GlobalTime);

		SetRange(attenEnd * 1.0f);
	}
}

glm::vec3 CM2Light::GetLightPosition() const
{
	return m_M2Light.GetPosition();
}
