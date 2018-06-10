#include "stdafx.h"

// General
#include "BoundingBox.h"

void BoundingBox::set(cvec3 _min, cvec3 _max, bool _needConvert)
{
	Min = _min;
	Max = _max;

	// Fix bounding box
	if (_needConvert)
	{
		Min = Min.toXZmY();
		Max = Max.toXZmY();
		std::swap(Min.z, Max.z);
	}

	//assert1(min.x < max.x && min.y < max.y && min.z < max.z);

	calculateInternal();
}

void BoundingBox::calculate(const vec3* _verts, uint32 _count, bool _needConvert)
{
	for (uint32 i = 0; i < _count; i++)
	{
		vec3 v = (_needConvert) ? (_verts[i].toXZmY()) : _verts[i];

		if (v.x < Min.x) Min.x = v.x;
		if (v.y < Min.y) Min.y = v.y;
		if (v.z < Min.z) Min.z = v.z;
		if (v.x > Max.x) Max.x = v.x;
		if (v.y > Max.y) Max.y = v.y;
		if (v.z > Max.z) Max.z = v.z;
	}

	calculateInternal();
}

void BoundingBox::calculateInternal()
{
	Center = (Min + Max) * 0.5f;
	Radius = (Max - Center).length();
}

void BoundingBox::clear()
{
	Min = Vec3f(0, 0, 0);
	Max = Vec3f(0, 0, 0);
	Center = Vec3f(0, 0, 0);
	Radius = 0.0f;
}

//

Vec3f BoundingBox::getCorner(uint32 index) const
{
	switch (index)
	{
	case 0:
		return Vec3f(Min.x, Min.y, Max.z);
	case 1:
		return Vec3f(Max.x, Min.y, Max.z);
	case 2:
		return Vec3f(Max.x, Max.y, Max.z);
	case 3:
		return Vec3f(Min.x, Max.y, Max.z);
	case 4:
		return Vec3f(Min.x, Min.y, Min.z);
	case 5:
		return Vec3f(Max.x, Min.y, Min.z);
	case 6:
		return Vec3f(Max.x, Max.y, Min.z);
	case 7:
		return Vec3f(Min.x, Max.y, Min.z);
	default:
		return Vec3f();
	}
}

void BoundingBox::transform(const Matrix4f& m)
{
	// Efficient algorithm for transforming an AABB, taken from Graphics Gems

	float minA[3] = { Min.x, Min.y, Min.z };
	float minB[3];
	float maxA[3] = { Max.x, Max.y, Max.z };
	float maxB[3];

	for (uint32 i = 0; i < 3; ++i)
	{
		minB[i] = m.c[3][i];
		maxB[i] = m.c[3][i];

		for (uint32 j = 0; j < 3; ++j)
		{
			float x = minA[j] * m.c[j][i];
			float y = maxA[j] * m.c[j][i];
			minB[i] += minf(x, y);
			maxB[i] += maxf(x, y);
		}
	}

	Min = Vec3f(minB[0], minB[1], minB[2]);
	Max = Vec3f(maxB[0], maxB[1], maxB[2]);

	Center = m * Center;
}

bool BoundingBox::makeUnion(const BoundingBox& b)
{
	bool changed = false;

	// Ignore zero-size boxes
	if (Min == Max)
	{
		changed = true;
		Min = b.Min;
		Max = b.Max;
	}
	else if (b.Min != b.Max)
	{
		if (b.Min.x < Min.x) { changed = true; Min.x = b.Min.x; }
		if (b.Min.y < Min.y) { changed = true; Min.y = b.Min.y; }
		if (b.Min.z < Min.z) { changed = true; Min.z = b.Min.z; }

		if (b.Max.x > Max.x) { changed = true; Max.x = b.Max.x; }
		if (b.Max.y > Max.y) { changed = true; Max.y = b.Max.y; }
		if (b.Max.z > Max.z) { changed = true; Max.z = b.Max.z; }
	}

	calculateInternal();

	return changed;
}
