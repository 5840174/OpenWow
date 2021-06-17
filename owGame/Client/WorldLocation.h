#pragma once

#if 0

struct ZN_API WorldLocation
	: public CSceneNode
{
	explicit WorldLocation(uint32 _mapid = 0, float _x = 0, float _y = 0, float _z = 0, float _o = 0)
		: mapid(_mapid)
		, PositionX(_x)
		, PositionY(_y)
		, PositionZ(_z)
		, Orientation(_o)
	{}

	WorldLocation(WorldLocation const &loc)
		: mapid(loc.mapid)
		, PositionX(loc.PositionX)
		, PositionY(loc.PositionY)
		, PositionZ(loc.PositionZ)
		, Orientation(loc.Orientation)
	{}

	uint32 mapid;
	float PositionX;
	float PositionY;
	float PositionZ;
	float Orientation;
};

#endif