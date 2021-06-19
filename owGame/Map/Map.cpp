#include "stdafx.h"

// General
#include "Map.h"

namespace
{
	bool IsHole(uint16 holes, uint16 i, uint16 j)
	{
		const uint16 holetab_h[4] = { 0x1111, 0x2222, 0x4444, 0x8888 };
		const uint16 holetab_v[4] = { 0x000F, 0x00F0, 0x0F00, 0xF000 };
		return (holes & holetab_h[i] & holetab_v[j]) != 0;
	}

	bool IsBadTileIndex(int i, int j)
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

	bool IsGoodTileIndex(int i, int j)
	{
		return (false == IsBadTileIndex(i, j));
	}
}

CMap::CMap(IScene& Scene)
	: CSceneNode(Scene)
{
	m_CurrentTileX = m_CurrentTileZ = -1;
	m_IsOnInvalidTile = false;

	{
		m_HighMapStrip = GenarateHighMapArray();
		m_DefaultMapStrip = GenarateDefaultMapArray();

		glm::vec4 detailAndAlphaTextureCoord[C_MapBufferSize];

		// init texture coordinates for detail map:
		glm::vec4* dtc = detailAndAlphaTextureCoord;
		const float detail_half = 0.5f * C_DetailSize / 8.0f;
		for (int j = 0; j < 17; j++)
		{
			for (int i = 0; i < ((j % 2) ? 8 : 9); i++)
			{
				float tx = C_DetailSize / 8.0f * i;
				float ty = C_DetailSize / 8.0f * j * 0.5f;
				if (j % 2)
				{
					tx += detail_half;
				}
				(*dtc++).xy = glm::vec2(tx, ty);
			}
		}

		// init texture coordinates for alpha map:
		glm::vec4* atc = detailAndAlphaTextureCoord;
		const float alpha_half = 0.5f * 1.0f / 8.0f;
		for (int j = 0; j < 17; j++)
		{
			for (int i = 0; i < ((j % 2) ? 8 : 9); i++)
			{
				float tx = 1.0f / 8.0f * i;
				float ty = 1.0f / 8.0f * j * 0.5f;
				if (j % 2)
				{
					tx += alpha_half;
				}
				(*atc++).zw = glm::vec2(tx, ty);
			}
		}

		m_BufferTextureCoordDetailAndAlpha = GetRenderDevice().GetObjectsFactory().CreateVertexBuffer(detailAndAlphaTextureCoord, C_MapBufferSize);
	}

	mProvider = nullptr;
	dir = nullptr;

	/*mProvider = new CMinimapProvider;
	mProvider->Init();

	dir = mProvider->getMinimap("Kalimdor");
	dir->Load();*/
}

CMap::~CMap()
{
}

// --

void CMap::MapPreLoad(const DBC_MapRecord* DBCMapRecord)
{
	m_MapDBCRecord = DBCMapRecord;

	Log::Print("Map[%s]: Id [%d]. Preloading...", m_MapDBCRecord->Get_Directory().c_str(), m_MapDBCRecord->Get_ID());

	m_WDL = std::make_unique<CMapWDL>(*this);
	m_WDL->Load();

	m_WDT = std::make_unique<CMapWDT>(*this);
}

void CMap::MapLoad()
{
	Log::Print("Map[%s]: Id [%d]. Loading...", m_MapDBCRecord->Get_Directory(), m_MapDBCRecord->Get_ID());

	m_WDL->CreateInsances(shared_from_this());

	m_WDT->Load();
	m_WDT->CreateInsances(shared_from_this());
}

void CMap::Unload()
{
	m_WDL.reset();
	m_WDT.reset();

	for (int i = 0; i < C_TilesCacheSize; i++)
	{
		if (m_MapTilesCache[i] != nullptr)
		{
			RemoveChild(m_MapTilesCache[i]);
		}
	}

	for (int i = 0; i < C_RenderedTiles; i++)
	{
		for (int j = 0; j < C_RenderedTiles; j++)
		{
			RemoveChild(m_MapTilesCurrent[i][j]);
		}
	}
}

// --

void CMap::Update(const UpdateEventArgs& e)
{
	const ICameraComponent3D* camera = e.CameraForCulling;

	bool loading = false;
	int enteredTileX = -1, enteredTileZ = -1;
	int midTile = static_cast<uint32>(C_RenderedTiles / 2);
	if (m_MapTilesCurrent[midTile][midTile] != nullptr || m_IsOnInvalidTile)
	{
		if (m_IsOnInvalidTile ||
			(camera->GetPosition().x < m_MapTilesCurrent[midTile][midTile]->GetPosition().x) ||
			(camera->GetPosition().x > (m_MapTilesCurrent[midTile][midTile]->GetPosition().x + C_TileSize)) ||
			(camera->GetPosition().z < m_MapTilesCurrent[midTile][midTile]->GetPosition().z) ||
			(camera->GetPosition().z > (m_MapTilesCurrent[midTile][midTile]->GetPosition().z + C_TileSize)))
		{

			enteredTileX = static_cast<int>(camera->GetPosition().x / C_TileSize);
			enteredTileZ = static_cast<int>(camera->GetPosition().z / C_TileSize);

			loading = true;
		}
	}

	if (loading)
	{
		if (enteredTileX != -1 && enteredTileZ != -1)
		{
			EnterMap(enteredTileX, enteredTileZ);
			enteredTileX = enteredTileZ = -1;
			loading = false;
		}
	}

	if (m_WDL)
		m_WDL->UpdateCamera(camera);
}

void CMap::EnterMap(glm::vec3 CameraPosition)
{
	EnterMap(CameraPosition.x / C_TileSize, CameraPosition.z / C_TileSize);
}

void CMap::EnterMap(int32 x, int32 z)
{
	if (IsBadTileIndex(x, z) || false == m_WDT->IsMapTileExists())
	{
		m_IsOnInvalidTile = true;
		return;
	}

	m_CurrentTileX = x;
	m_CurrentTileZ = z;

	for (uint8 i = 0; i < C_RenderedTiles; i++)
	{
		for (uint8 j = 0; j < C_RenderedTiles; j++)
		{
			m_MapTilesCurrent[i][j] = LoadTile(x - (C_RenderedTiles / 2u) + i, z - (C_RenderedTiles / 2u) + j);
		}
	}
}

std::shared_ptr<CMapTile> CMap::LoadTile(int32 x, int32 z)
{
	if (IsBadTileIndex(x, z))
	{
		return nullptr;
	}

	if (false == m_WDT->IsMapTileExists())
	{
		return nullptr;
	}

	// Try get tile from cache
	uint32 freePositionInTileCache = C_TilesCacheSize;

	for (uint32 i = 0; i < C_TilesCacheSize; i++)
	{
		if ((m_MapTilesCache[i] != nullptr) && (m_MapTilesCache[i]->getIndexX() == x) && (m_MapTilesCache[i]->getIndexZ() == z))
		{
			return m_MapTilesCache[i];
		}

		if ((m_MapTilesCache[i] == nullptr) && (i < freePositionInTileCache))
		{
			freePositionInTileCache = i;
		}
	}

	// ok we need to find a place in the cache
	if (freePositionInTileCache == C_TilesCacheSize)
	{
		int score, maxscore = 0, maxidx = 0;

		// oh shit we need to throw away a tile
		for (int i = 0; i < C_TilesCacheSize; i++)
		{
			auto currentTileInCache = m_MapTilesCache[i];
			if (currentTileInCache == nullptr)
				continue;

			score = glm::abs(currentTileInCache->getIndexX() - m_CurrentTileX) + glm::abs(currentTileInCache->getIndexZ() - m_CurrentTileZ);

			if (score > maxscore)
			{
				maxscore = score;
				maxidx = i;
			}
		}

		// maxidx is the winner (loser)
		RemoveChild(m_MapTilesCache[maxidx]);
		GetBaseManager().GetManager<ILoader>()->AddToDeleteQueue(m_MapTilesCache[maxidx]);
		freePositionInTileCache = maxidx;
	}

	// Create new tile
	auto newTile = CreateSceneNode<CMapTile>(*this, x, z);
	GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(newTile);

	m_MapTilesCache[freePositionInTileCache] = newTile;

	return newTile;
}

void CMap::ClearCache()
{
	for (uint32 i = 0; i < C_TilesCacheSize; i++)
	{
		if (m_MapTilesCache[i] != nullptr && false == IsTileInCurrent(*m_MapTilesCache[i]))
		{
			m_MapTilesCache[i] = 0;
		}
	}
}

uint32 CMap::GetAreaID(glm::vec3 CameraPosition)
{
	if (false == m_WDT->IsMapTileExists())
		return UINT32_MAX;

	int32 tileX = (int)(CameraPosition.x / C_TileSize);
	int32 tileZ = (int)(CameraPosition.z / C_TileSize);

	int32 chunkX = (int)(glm::mod(CameraPosition.x, C_TileSize) / C_ChunkSize);
	int32 chunkZ = (int)(glm::mod(CameraPosition.z, C_TileSize) / C_ChunkSize);

	if ((tileX < m_CurrentTileX - static_cast<int32>(C_RenderedTiles / 2)) ||
		(tileX > m_CurrentTileX + static_cast<int32>(C_RenderedTiles / 2)) ||
		(tileZ < m_CurrentTileZ - static_cast<int32>(C_RenderedTiles / 2)) ||
		(tileZ > m_CurrentTileZ + static_cast<int32>(C_RenderedTiles / 2)))
	{
		return UINT32_MAX;
	}

	int32 indexX = tileZ - m_CurrentTileZ + static_cast<int32>(C_RenderedTiles / 2);
	int32 indexY = tileX - m_CurrentTileX + static_cast<int32>(C_RenderedTiles / 2);
	auto curTile = m_MapTilesCurrent[indexX][indexY];
	if (curTile == nullptr)
	{
		return UINT32_MAX;
	}

	const CMapChunk* curChunk = curTile->getChunk(chunkX, chunkZ);
	if (curChunk == nullptr)
	{
		return UINT32_MAX;
	}

	return curChunk->GetAreaID();
}


//
// Public (Getters)
//
std::string CMap::GetMapFolder() const
{
	return "World\\Maps\\" + std::string(m_MapDBCRecord->Get_Directory()) + "\\" + std::string(m_MapDBCRecord->Get_Directory());
}

bool CMap::IsNortrend() const
{
	return m_MapDBCRecord->Get_Expansion() == 2;
}

bool CMap::getTileIsCurrent(int x, int z) const
{
    int midTile = static_cast<uint32>(C_RenderedTiles / 2);
    auto currentTile = m_MapTilesCurrent[midTile][midTile];
    if (currentTile == nullptr)
    {
        return false;
    }

    int32 currentX = currentTile->getIndexX();
    int32 currentZ = currentTile->getIndexZ();

    return (
        x >= (currentX - (C_RenderedTiles / 2)) &&
        z >= (currentZ - (C_RenderedTiles / 2)) &&
        x <= (currentX + (C_RenderedTiles / 2)) &&
        z <= (currentZ + (C_RenderedTiles / 2))
        );
}

bool CMap::IsTileInCurrent(const CMapTile& _mapTile)
{
	for (int i = 0; i < C_RenderedTiles; i++)
		for (int j = 0; j < C_RenderedTiles; j++)
			if (m_MapTilesCurrent[i][j] != nullptr)
				if (m_MapTilesCurrent[i][j].get() == &_mapTile)
					return true;
	return false;
}




//
// Public (MapShared)
//
std::vector<uint16> CMap::GenarateHighMapArray(uint16 _holes) const
{
	if (_holes == 0 && false == m_HighMapStrip.empty())
	{
		return m_HighMapStrip;
	}

	int16 outerArray[9][9];
	for (uint16 i = 0; i < 9; i++)
		for (uint16 j = 0; j < 9; j++)
			outerArray[i][j] = (i * 17) + j;

	int16 innerArray[8][8];
	for (uint16 i = 0; i < 8; i++)
		for (uint16 j = 0; j < 8; j++)
			innerArray[i][j] = 9 + (i * 17) + j;

	std::vector<uint16> myIndexes;
	for (uint16 i = 0; i < 8; i++)
	{
		for (uint16 j = 0; j < 8; j++)
		{
			if (IsHole(_holes, j / 2, i / 2))
			{
				continue;
			}

			myIndexes.push_back(outerArray[i][j]);
			myIndexes.push_back(innerArray[i][j]);
			myIndexes.push_back(outerArray[i][j + 1]);

			myIndexes.push_back(outerArray[i][j + 1]);
			myIndexes.push_back(innerArray[i][j]);
			myIndexes.push_back(outerArray[i + 1][j + 1]);

			myIndexes.push_back(outerArray[i + 1][j + 1]);
			myIndexes.push_back(innerArray[i][j]);
			myIndexes.push_back(outerArray[i + 1][j]);

			myIndexes.push_back(outerArray[i + 1][j]);
			myIndexes.push_back(innerArray[i][j]);
			myIndexes.push_back(outerArray[i][j]);
		}
	}

	return myIndexes;
}

std::vector<uint16> CMap::GenarateDefaultMapArray(uint16 _holes) const
{
	if (_holes == 0 && false == m_DefaultMapStrip.empty())
	{
		return m_DefaultMapStrip;
	}

	int16 outerArray[9][9];
	for (uint16 i = 0; i < 9; i++)
		for (uint16 j = 0; j < 9; j++)
			outerArray[i][j] = (i * 17) + j;

	std::vector<uint16> myIndexes;
	for (uint16 i = 0; i < 8; i += 1)
	{
		for (uint16 j = 0; j < 8; j += 1)
		{
			if (IsHole(_holes, j / 2, i / 2))
			{
				continue;
			}

			myIndexes.push_back(outerArray[i][j]);
			myIndexes.push_back(outerArray[i + 1][j]);
			myIndexes.push_back(outerArray[i][j + 1]);

			myIndexes.push_back(outerArray[i][j + 1]);
			myIndexes.push_back(outerArray[i + 1][j]);
			myIndexes.push_back(outerArray[i + 1][j + 1]);
		}
	}

	return myIndexes;
}

std::shared_ptr<IBuffer> CMap::GetBufferTextureCoordDetailAndAlpha() const
{
	return m_BufferTextureCoordDetailAndAlpha;
}
