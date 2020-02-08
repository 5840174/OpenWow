#include "stdafx.h"

// General
#include "M2_Part_Color.h"

CM2_Part_Color::CM2_Part_Color(std::shared_ptr<IFile> f, const SM2_Color& _proto, cGlobalLoopSeq global) :
	vColor(vec3(1.0f, 1.0f, 1.0f)),
	vAlpha(1.0f)
{
	color.init(_proto.color, f, global);
	alpha.init(_proto.alpha, f, global);
}

void CM2_Part_Color::calc(uint16 anim, uint32 time, uint32 globalTime)
{
	if (color.uses(anim))
	{
		vColor = color.getValue(anim, time, globalTime);
	}

	if (alpha.uses(anim))
	{
		vAlpha = alpha.getValue(anim, time, globalTime);
	}
}
