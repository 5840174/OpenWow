#include "stdafx.h"

// General
#include "M2_Part_TextureWeight.h"

CM2_Part_TextureWeight::CM2_Part_TextureWeight(std::shared_ptr<IFile> f, const SM2_TextureWeight& _proto, cGlobalLoopSeq global) :
	tVal(1.0f)
{
	trans.init(_proto.weight, f, global);
}

void CM2_Part_TextureWeight::calc(uint16 anim, uint32 time, uint32 globalTime)
{
	if (trans.uses(anim))
	{
		tVal = trans.getValue(anim, time, globalTime);
	}
}