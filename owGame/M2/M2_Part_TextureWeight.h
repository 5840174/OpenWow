#pragma once

#include "M2_Types.h"

struct CM2_Part_TextureWeight
{
public:
	CM2_Part_TextureWeight(std::shared_ptr<IFile> f, const SM2_TextureWeight& _proto, cGlobalLoopSeq global);

	void calc(uint16 anim, uint32 time, uint32 globalTime);

	float getValue() const { return tVal; }

private:
	M2_Animated<float, short, ShortToFloat> trans;
	float tVal;
};