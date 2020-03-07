#include "stdafx.h"

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Part_Light.h"

SM2_Part_Light_Wrapper::SM2_Part_Light_Wrapper(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Light& M2Light) 
	: m_M2Object(M2Object)
	, m_M2Light(M2Light)
{
	ambColor.Initialize(M2Light.ambient_color, File);
	ambIntensity.Initialize(M2Light.ambient_intensity, File);

	diffColor.Initialize(M2Light.diffuse_color, File);
	diffIntensity.Initialize(M2Light.diffuse_intensity, File);

	attenuation_start.Initialize(M2Light.attenuation_start, File);
	attenuation_end.Initialize(M2Light.attenuation_end, File);

	visibility.Initialize(M2Light.visibility, File);
}

SM2_Part_Light_Wrapper::~SM2_Part_Light_Wrapper()
{
}

SLight SM2_Part_Light_Wrapper::GetLight(const CM2_Base_Instance * M2Instance, uint32 globalTime) const
{
	SLight lightStruct;
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
			lightStruct.Color.rgb = diffColor.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		if (diffIntensity.IsUsesBySequence(animator->getSequenceIndex()))
			lightStruct.Intensity = diffIntensity.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);

		if (attenuation_start.IsUsesBySequence(animator->getSequenceIndex()))
			attenStart = attenuation_start.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		if (attenuation_end.IsUsesBySequence(animator->getSequenceIndex()))
			attenEnd = attenuation_end.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), globalTime);
		lightStruct.Range = attenEnd;
	}

	return lightStruct;
}
