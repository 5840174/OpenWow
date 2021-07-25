#pragma once

#include "WowConsts.h"

static inline glm::vec3 Fix_XZY(const glm::vec3& Vec)
{
	return glm::vec3(Vec.x, Vec.z, Vec.y);
}

static inline glm::vec3 Fix_From_XZmY_To_XYZ(const glm::vec3& Vec)
{
	return glm::vec3(Vec.x,           Vec.z, (-1.0f) * Vec.y);
}

static inline glm::vec3 Fix_From_XYZ_to_XZmY(const glm::vec3& Vec)
{
	return glm::vec3(Vec.x, (-1.0f) * Vec.z,           Vec.y);
}

static inline glm::quat Fix_XZmYW(const glm::quat& _quat)
{
	return glm::quat(_quat.w, _quat.x, _quat.z, -_quat.y);
}


inline inline BoundingBox Fix_From_XZmY_To_XYZ(BoundingBox BBOX)
{
	glm::vec3 boundsMin = Fix_From_XZmY_To_XYZ(BBOX.getMin());
	glm::vec3 boundsMax = Fix_From_XZmY_To_XYZ(BBOX.getMax());
	std::swap(boundsMin.z, boundsMax.z);
	return BoundingBox(boundsMin, boundsMax);
}

static inline BoundingBox Fix_From_XYZ_to_XZmY(BoundingBox BBOX)
{
	glm::vec3 boundsMin = Fix_From_XYZ_to_XZmY(BBOX.getMin());
	glm::vec3 boundsMax = Fix_From_XYZ_to_XZmY(BBOX.getMax());
	std::swap(boundsMin.y, boundsMax.y);
	return BoundingBox(boundsMin, boundsMax);
}



static inline glm::vec3 fromRealToGame(const glm::vec3& p)
{
	return glm::vec3(
		-p.x + C_ZeroPoint,
		-p.z + C_ZeroPoint,
		(p.y)
	);
}

// Y            X        Z			// DBC
//-618.518, -4251.67, 38.718, 0
// X			Y        Z
//-4251.67, -618.518, 38.718, 0
static inline glm::vec3 fromGameToReal(const glm::vec3& p)
{
	return glm::vec3(
		-p.y + C_ZeroPoint,
		p.z,
		-p.x + C_ZeroPoint
	);
}


inline ColorRGBA fromARGB(uint32 color)
{
	const float a = ((color & 0xFF000000) >> 24) / 255.0f;
	const float r = ((color & 0x00FF0000) >> 16) / 255.0f;
	const float g = ((color & 0x0000FF00) >> 8) / 255.0f;
	const float b = ((color & 0x000000FF)) / 255.0f;
	return ColorRGBA(r, g, b, a);
}

static inline ColorRGB fromRGB(uint32 color)
{
	const float r = ((color & 0xFF0000) >> 16) / 255.0f;
	const float g = ((color & 0x00FF00) >> 8) / 255.0f;
	const float b = ((color & 0x0000FF)) / 255.0f;
	return ColorRGB(r, g, b);
}

static inline ColorRGBA fromBGRA(uint32 color)
{
	const float b = ((color & 0xFF000000) >> 24) / 255.0f;
	const float g = ((color & 0x00FF0000) >> 16) / 255.0f;
	const float r = ((color & 0x0000FF00) >> 8) / 255.0f;
	const float a = ((color & 0x000000FF)) / 255.0f;
	return ColorRGBA(r, g, b, a);
}

static inline ColorRGBA fromABGR(uint32 color)
{
	const float a = ((color & 0xFF000000) >> 24) / 255.0f;
	const float b = ((color & 0x00FF0000) >> 16) / 255.0f;
	const float g = ((color & 0x0000FF00) >> 8) / 255.0f;
	const float r = ((color & 0x000000FF)) / 255.0f;
	return ColorRGBA(r, g, b, a);
}

#pragma pack(push,1)

struct C4Plane
{
	glm::vec3 normal;
	float distance;
};

struct C4ImQuaternion
{
	float w;
	float x;
	float y;
	float z;
};

struct CRange
{
	float min;
	float max;
};

struct CAaBox
{
	glm::vec3 min;
	glm::vec3 max;

	inline BoundingBox Convert() const
	{
		glm::vec3 boundsMin = Fix_From_XZmY_To_XYZ(min);
		glm::vec3 boundsMax = Fix_From_XZmY_To_XYZ(max);
		std::swap(boundsMin.z, boundsMax.z);
		return BoundingBox(boundsMin, boundsMax);
	}
};

struct CAaSphere
{
	glm::vec3 position;
	float radius;
};

struct CArgb
{
	CArgb() : r(0), g(0), b(0), a(0) {}
	CArgb(uint8 _r, uint8 _g, uint8 _b, uint8 _a) : r(_r), g(_g), b(_b), a(_a) {}

	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
};

struct CBgra
{
	CBgra() : b(0), g(0), r(0), a(0) {}
	CBgra(uint8 _b, uint8 _g, uint8 _r, uint8 _a) : b(_b), g(_g), r(_r), a(_a) {}

	uint8 b;
	uint8 g;
	uint8 r;
	uint8 a;
};

struct C3sVector
{
	int16 x;
	int16 y;
	int16 z;
};

struct C4Vec
{
	uint8 x;
	uint8 y;
	uint8 z;
	uint8 w;
};

struct C4Quaternion
{
	float x;
	float y;
	float z;
	float w; // Unlike Quaternions elsewhere, the scalar part ('w') is the last element in the struct instead of the first
};

struct C3Segment
{
	glm::vec3 start;
	glm::vec3 end;
};

struct CFacet
{
	C4Plane plane;
	glm::vec3 vertices[3];
};

struct C3Ray
{
	glm::vec3 origin;
	glm::vec3 dir;
};

#pragma pack(pop)

