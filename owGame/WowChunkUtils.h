#pragma once

inline static void flipcc(char* fcc)
{
	std::swap(fcc[0], fcc[3]);
	std::swap(fcc[1], fcc[2]);
}

#define WOWCHUNK_READ_STRINGS_BEGIN \
char* __tbuf = new char[size + 1]; \
f->readBytes(__tbuf, size); \
__tbuf[size] = 0; \
char* __tp = __tbuf; \
while (__tp < __tbuf + size) \
{ \
	std::string _string(__tp); \
	__tp += _string.length() + 1; \

#define WOWCHUNK_READ_STRINGS_END \
} \
delete[] __tbuf;


#define WOWCHUNK_READ_STRINGS2_BEGIN \
char* p = (char*)(f->getDataFromCurrent()); \
char* end = p + size; \
int t = 0; \
while (p < end) \
{ \
	std::string _string(p); \
	p += strlen(p) + 1; \
	while ((p < end) && (*p == 0)) p++; \

#define WOWCHUNK_READ_STRINGS2_END \
} \
f->seekRelative(size);


static vec3 fromRealToGame(vec3 p)
{
	return vec3(
		-p.x + C_ZeroPoint,
		-p.z + C_ZeroPoint,
		(p.y)
	);
}

// Y            X        Z			// DBC
//-618.518, -4251.67, 38.718, 0
// X			Y        Z
//-4251.67, -618.518, 38.718, 0

static vec3 fromGameToReal(vec3 p)
{
	return vec3(
		-p.x + C_ZeroPoint,
		p.z,
		-p.y + C_ZeroPoint
	);
}