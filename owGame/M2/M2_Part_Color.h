#pragma once

#include "M2_Types.h"

class CM2_Part_Color
{
public:
	CM2_Part_Color(std::shared_ptr<IFile> f, const SM2_Color& _proto, cGlobalLoopSeq global);

	void calc(uint16 anim, uint32 time, uint32 globalTime);

	vec4 getValue() const { return vec4(vColor, vAlpha); }

	cvec3 getValueColor() const { return vColor; }
	float getValueAlpha() const { return vAlpha; }

private:
	M2_Animated<vec3> color;
	M2_Animated<float, short, ShortToFloat> alpha;

	vec3 vColor;
	float vAlpha;
};