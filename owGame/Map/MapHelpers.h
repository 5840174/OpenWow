#pragma once

inline bool IsHole(uint16 holes, uint16 i, uint16 j)
{
	const uint16 holetab_h[4] = { 0x1111, 0x2222, 0x4444, 0x8888 };
	const uint16 holetab_v[4] = { 0x000F, 0x00F0, 0x0F00, 0xF000 };
	return (holes & holetab_h[i] & holetab_v[j]) != 0;
}

inline bool IsBadTileIndex(int32 i, int32 j)
{
	if (i < 0)
		return true;

	if (j < 0)
		return true;

	if (i >= C_TilesInMap)
		return true;

	if (j >= C_TilesInMap)
		return true;

	return false;
}

inline bool IsGoodTileIndex(int32 i, int32 j)
{
	return (false == IsBadTileIndex(i, j));
}

inline uint16 GetOuterMapChunkArrayIndex(uint16 X, uint16 Z)
{
	return (X * 17u) + Z;
}

inline uint16 GetInnerMapChunkArrayIndex(uint16 X, uint16 Z)
{
	return 9u + (X * 17u) + Z;
}