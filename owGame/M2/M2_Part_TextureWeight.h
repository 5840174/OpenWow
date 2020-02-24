#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class M2;
// FORWARD END

struct CM2_Part_TextureWeight
{
public:
	CM2_Part_TextureWeight(const M2& M2Object, const std::shared_ptr<IFile>& File, const SM2_TextureWeight& M2TextureWeight);
	virtual ~CM2_Part_TextureWeight();

	float GetWeight(uint16 Sequence, uint32 Time, uint32 GlobalTime) const;

private:
	M2_Animated<float, short, ShortToFloat> m_WeightAnimated;

	const M2& m_M2Object;
};