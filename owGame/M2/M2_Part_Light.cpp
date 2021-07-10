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
