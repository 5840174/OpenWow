#pragma once

#pragma pack(push,1)

template<typename T>
struct M2Array
{
    uint32 size;
    uint32 offset;
};

struct M2TrackBase
{
    Interpolations						interpolation_type;
    int16								global_sequence;

#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5
    M2Array<std::pair<uint32, uint32>>	interpolation_ranges;
    M2Array<uint32>                     timestamps;
#else
	M2Array<M2Array<uint32_t>>          timestamps;
#endif
};

template<typename T>
struct M2Track : public M2TrackBase
{
#if WOW_CLIENT_VERSION < WOW_WOTLK_3_3_5
    M2Array<T>           values;
#else
	M2Array<M2Array<T>>  values;
#endif
};

template<typename T>
struct M2TrackFake
{
    M2Array<uint32>                    timestamps;
    M2Array<T>                         values;
};

struct M2CompQuat
{
    int16 x, y, z, w;
};

struct M2Bounds
{
    CAaBox extent;
    float radius;
};

struct M2Box
{
    glm::vec3 ModelRotationSpeedMin;
	glm::vec3 ModelRotationSpeedMax;
};

struct M2Range
{
    uint32 minimum;
    uint32 maximum;
};

#pragma pack(pop)
