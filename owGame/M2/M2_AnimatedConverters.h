#pragma once

#include "M2_CommonTypes.h"

// interpolation functions

template<class T>
inline T interpolateNone(const float r, const T& v1, const T& v2)
{
	return r > 0.5f ? v2 : v1;
}

template<class T>
inline T interpolateLinear(const float r, const T& v1, const T& v2)
{
	return v1 * (1.0f - r) + v2 * r;
}

// "linear" interpolation for quaternions should be slerp by default
template<>
inline glm::quat interpolateLinear<glm::quat>(const float r, const glm::quat& v1, const glm::quat& v2)
{
	return glm::slerp(v1, v2, r);
}

template<class T>
inline T interpolateHermite(const float r, const T& v1, const T& v2, const T& in, const T& out)
{
	const float rPow3 = glm::pow(r, 3);
	const float rPow2 = glm::pow(r, 2);

	// basis functions
	const float h1 = 2.0f * rPow3 - 3.0f * rPow2 + 1.0f;
	const float h2 = -2.0f * rPow3 + 3.0f * rPow2;
	const float h3 = rPow3 - 2.0f * rPow2 + r;
	const float h4 = rPow3 - rPow2;

	// interpolation
	return v1 * h1 + v2 * h2 + in * h3 + out * h4;
}

template<class T>
inline T interpolateBezier(const float r, const T& v1, const T& v2, const T& in, const T& out)
{
	const float InverseFactor = (1.0f - r);
	const float FactorTimesTwo = r * r;
	const float InverseFactorTimesTwo = InverseFactor * InverseFactor;

	// basis functions
	const float h1 = InverseFactorTimesTwo * InverseFactor;
	const float h2 = 3.0f * r * InverseFactorTimesTwo;
	const float h3 = 3.0f * FactorTimesTwo * InverseFactor;
	const float h4 = FactorTimesTwo * r;

	// interpolation
	return static_cast<T>((v1 * h1) + (v2 * h2) + (in * h3) + (out * h4));
}

template<class T>
static inline T NoFix(const T& _v)
{
	return _v;
}

template <class T>
class NoConvert
{
public:
	static const T& conv(const T& t)
	{
		return t;
	}
};

class Quat16ToQuat32
{
public:
	static const glm::quat conv(const M2CompQuat t)
	{
		return glm::quat(
			float(t.w > 0 ? t.w - 32767i16 : t.w + 32767i16) / 32767.0f,
			float(t.x > 0 ? t.x - 32767i16 : t.x + 32767i16) / 32767.0f,
			float(t.y > 0 ? t.y - 32767i16 : t.y + 32767i16) / 32767.0f,
			float(t.z > 0 ? t.z - 32767i16 : t.z + 32767i16) / 32767.0f);
	}
};

class ShortToFloat
{
public:
	static const float conv(const short t)
	{
		return static_cast<float>(t) / 32767.0f;
	}
};