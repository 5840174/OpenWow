#pragma once

// Tiles
const uint32 C_TilesInMap = 64u;

#ifdef _DEBUG
const uint32 C_RenderedTiles = 1u;
const uint32 C_TilesCacheSize = ((C_RenderedTiles + 1u) * (C_RenderedTiles + 1u));
#else
const uint32 C_RenderedTiles = 3u;
const uint32 C_TilesCacheSize = ((C_RenderedTiles + 1u) * (C_RenderedTiles + 1u));
#endif

const float C_TileSize = 533.3333333333f;
const float C_ChunkSize = C_TileSize / 16.0f;
const float C_UnitSize = C_ChunkSize / 8.0f;
const float C_ZeroPoint = 32.0f * C_TileSize; // 17066.66656

// Tile chunk
const int32 C_ChunksInTile = 16;
const int32 C_ChunksInTileGlobal = C_ChunksInTile * C_ChunksInTile;
const int32 C_MapBufferSize = 9 * 9 + 8 * 8;

// World
const float C_DetailSize = 8.0f;
const float C_AlphaSize = 1.0f;

