#pragma once

// Tiles
const uint32 C_TilesInMap = 64;

#ifdef _DEBUG
const int32 C_RenderedTiles = 1;
const int32 C_TilesCacheSize = ((C_RenderedTiles + 1) * (C_RenderedTiles + 1));
#else
const int32 C_RenderedTiles = 3;
const int32 C_TilesCacheSize = ((C_RenderedTiles + 1) * (C_RenderedTiles + 1));
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

// M2 models
const uint8	cM2Model_TexturesMaxCount = 128;
const uint8	cM2Model_BonesInfluences = 4;

// Types & Consts
const ObjectType cSky_NodeType               = 1000;

const ObjectType cMap_NodeType               = 1010;
const ObjectType cMapLowResTile_NodeType     = 1011;
const ObjectType cMapTile_NodeType           = 1012;
const ObjectType cMapChunk_NodeType          = 1013;

const ObjectType cLiquid_NodeType            = 1020;
const ObjectType cLiquid_MapChnuk_NodeType   = 1021;
const ObjectType cLiquid_WMOGroup_NodeType   = 1022;

const ObjectType cWMO_NodeType               = 1030;
const ObjectType cWMOGroup_NodeType          = 1031;

const ObjectType cM2_NodeType                = 1040;
