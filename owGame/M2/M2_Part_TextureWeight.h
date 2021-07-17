#pragma once

#include "M2_Types.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

struct ZN_API CM2_Part_TextureWeight
{
public:
	CM2_Part_TextureWeight(const CM2& M2Object, const std::shared_ptr<IByteBuffer>& File, const SM2_TextureWeight& M2TextureWeight);
	virtual ~CM2_Part_TextureWeight();

	float GetWeight(const CM2_Base_Instance* M2Instance, uint32 GlobalTime) const;

private:
	M2_Animated<float, short, ShortToFloat> m_WeightAnimated;

	const CM2& m_M2Object;
};
