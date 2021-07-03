#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Part_Light.h"

CM2_Part_Light::CM2_Part_Light(const CM2& M2Object, const std::shared_ptr<IByteBuffer>& ByteBuffer, const SM2_Light& M2Light)
	: m_M2Object(M2Object)
	, m_M2Light(M2Light)
{
	ambColor.Initialize(M2Light.ambient_color, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());
	ambIntensity.Initialize(M2Light.ambient_intensity, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());

	diffColor.Initialize(M2Light.diffuse_color, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());
	diffIntensity.Initialize(M2Light.diffuse_intensity, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());

	attenuation_start.Initialize(M2Light.attenuation_start, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());
	attenuation_end.Initialize(M2Light.attenuation_end, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());

	visibility.Initialize(M2Light.visibility, ByteBuffer, M2Object.getSkeleton().GetAnimFiles());
}

CM2_Part_Light::~CM2_Part_Light()
{
}

std::shared_ptr<ILight> CM2_Part_Light::GetLight(const CM2_Base_Instance * M2Instance, uint32 globalTime) const
{
	std::shared_ptr<CLight> lightStruct = MakeShared(CLight, m_M2Object.GetBaseManager());

	if (m_M2Light.type == SM2_Light::Type::Directional)
	{
		// TODO: rotation used world pos of node + rotate_bone * pos
	}
	else if (m_M2Light.type == SM2_Light::Type::Point)
	{
		// TODO: position used world pos of node + transform_bone * pos
	}
	else
	{
		_ASSERT(false);
	}

	//glm::vec4 ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float attenStart = 1.0f, attenEnd = 150.0f;

	if (const auto& animator = M2Instance->getAnimator())
	{
		//if (ambColor.IsUsesBySequence(animator->getSequenceIndex()))
		//	ambient.rgb = ambColor.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		//if (ambIntensity.IsUsesBySequence(animator->getSequenceIndex()))
		//	ambient.a = ambIntensity.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		
		if (diffColor.IsUsesBySequence(animator->getSequenceIndex()))
			lightStruct->SetColor(ColorRGB(diffColor.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime)));
		
		if (diffIntensity.IsUsesBySequence(animator->getSequenceIndex()))
			lightStruct->SetIntensity(diffIntensity.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime));

		if (attenuation_start.IsUsesBySequence(animator->getSequenceIndex()))
			attenStart = attenuation_start.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		if (attenuation_end.IsUsesBySequence(animator->getSequenceIndex()))
			attenEnd = attenuation_end.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

		lightStruct->SetIntensity(attenEnd);
	}

	return lightStruct;
}
