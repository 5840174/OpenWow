#include "stdafx.h"

// Include
#include "M2.h"

// General
#include "M2_Part_TextureWeight.h"

CM2_Part_TextureWeight::CM2_Part_TextureWeight(const M2& M2Object, const std::shared_ptr<IFile>& File, const SM2_TextureWeight& M2TextureWeight)
	: m_M2Object(M2Object)
{
	m_WeightAnimated.Initialize(M2TextureWeight.weight, File);
}

CM2_Part_TextureWeight::~CM2_Part_TextureWeight()
{
}

float CM2_Part_TextureWeight::GetWeight(uint16 Sequence, uint32 Time, uint32 GlobalTime) const
{
	if (m_WeightAnimated.IsUsesBySequence(Sequence))
		return m_WeightAnimated.GetValue(Sequence, Time, m_M2Object.getGlobalLoops(), GlobalTime);

	return 1.0f;
}