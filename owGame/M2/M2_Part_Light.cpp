#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_Light.h"

CM2_Part_Light::CM2_Part_Light(const M2& M2Object, const std::shared_ptr<IFile>& File, const SM2_Light& M2Light) 
	: ambColorValue(vec3(1.0f, 1.0f, 1.0f))
	, ambIntensityValue(1.0f)
	, diffColorValue(vec3(1.0f, 1.0f, 1.0f))
	, diffIntensityValue(1.0f)
	, attenuation_startValue(0.0f)
	, attenuation_endValue(1.0f)
	, visibilityValue(false)
	, m_M2Object(M2Object)
{
	type = M2Light.type;
	if (M2Light.bone != -1)
		m_Bone = m_M2Object.getSkeleton()->getBoneLookup(M2Light.bone);

	position = Fix_XZmY(M2Light.position);

	ambColor.Initialize(M2Light.ambient_color, File);
	ambIntensity.Initialize(M2Light.ambient_intensity, File);

	diffColor.Initialize(M2Light.diffuse_color, File);
	diffIntensity.Initialize(M2Light.diffuse_intensity, File);

	attenuation_start.Initialize(M2Light.attenuation_start, File);
	attenuation_end.Initialize(M2Light.attenuation_end, File);

	visibility.Initialize(M2Light.visibility, File);
}

CM2_Part_Light::~CM2_Part_Light()
{
}

void CM2_Part_Light::setup(uint16 anim, uint32 time, uint32 globalTime)
{
	if (ambColor.IsUsesBySequence(anim))
	{
		ambColorValue = ambColor.GetValue(0, time, m_M2Object.getGlobalLoops(), globalTime);
	}
	if (ambIntensity.IsUsesBySequence(anim))
	{
		ambIntensityValue = ambIntensity.GetValue(0, time, m_M2Object.getGlobalLoops(), globalTime);
	}

	if (diffColor.IsUsesBySequence(anim))
	{
		diffColorValue = diffColor.GetValue(0, time, m_M2Object.getGlobalLoops(), globalTime);
	}
	if (diffIntensity.IsUsesBySequence(anim))
	{
		diffIntensityValue = diffIntensity.GetValue(0, time, m_M2Object.getGlobalLoops(), globalTime);
	}

	vec4 ambcol(ambColorValue * ambIntensityValue, 1.0f);
	vec4 diffcol(diffColorValue * diffIntensityValue, 1.0f);

	std::shared_ptr<const CM2_Part_Bone> Bone = m_Bone.lock();
	if (Bone != nullptr)
	{
		if (type == SM2_Light::Type::Directional)
		{
			positionValue = Bone->getTransformMatrix() * vec4(position, 0);
		}
		else if (type == SM2_Light::Type::Point)
		{
			directionValue = Bone->getRotateMatrix() * vec4(direction, 0);
		}		
	}

	//Log::Info("Light %d (%f,%f,%f) (%f,%f,%f) [%f,%f,%f]", l-GL_LIGHT4, ambcol.x, ambcol.y, ambcol.z, diffcol.x, diffcol.y, diffcol.z, p.x, p.y, p.z);
	//glLightfv(l, GL_POSITION, p);
	//glLightfv(l, GL_DIFFUSE, diffcol);
	//glLightfv(l, GL_AMBIENT, ambcol);
	//glEnable(l);
}