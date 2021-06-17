#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "M2.h"
#include "M2_Base_Instance.h"

// General
#include "M2_Part_TextureWeight.h"

CM2_Part_TextureWeight::CM2_Part_TextureWeight(const CM2& M2Object, const std::shared_ptr<IFile>& File, const SM2_TextureWeight& M2TextureWeight)
	: m_M2Object(M2Object)
{
	m_WeightAnimated.Initialize(M2TextureWeight.weight, File, M2Object.getSkeleton().GetAnimFiles());
}

CM2_Part_TextureWeight::~CM2_Part_TextureWeight()
{
}

float CM2_Part_TextureWeight::GetWeight(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const
{
	if (const auto& animator = M2Instance->getAnimator())
		if (m_WeightAnimated.IsUsesBySequence(animator->getSequenceIndex()))
			return m_WeightAnimated.GetValue(animator->getSequenceIndex(), animator->getCurrentTime(), m_M2Object.getSkeleton().getGlobalLoops(), GlobalTime);
	return 1.0f;
}

#endif
