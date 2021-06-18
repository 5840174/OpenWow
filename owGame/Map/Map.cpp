#include "stdafx.h"

// General
#include "Map.h"

namespace
{
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
		return (!IsBadTileIndex(i, j));
	}
}

CMap::CMap(IScene& Scene)
	: CSceneNode(Scene)
{
	m_CurrentTileX = m_CurrentTileZ = -1;
	m_IsOnInvalidTile = false;

	if (_MapShared == nullptr)
	{
		_MapShared = new CMapShared(GetRenderDevice());
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
	delete _MapShared;
}

// --

void CMap::MapPreLoad(const DBC_MapRecord* DBCMapRecord)
{
	m_MapDBCRecord = DBCMapRecord;
	m_MapFolderName = CMapShared::getMapFolder(m_MapDBCRecord);

	Log::Print("Map[%s]: Id [%d]. Preloading...", m_MapDBCRecord->Get_Directory().c_str(), m_MapDBCRecord->Get_ID());

	m_WDL = std::make_unique<CMapWDL>(GetBaseManager(), GetRenderDevice(), *this);
	m_WDL->Load();

	m_WDT = std::make_unique<CMapWDT>(GetBaseManager(), GetRenderDevice(), *this);
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

//--

void CMap::EnterMap(glm::vec3 CameraPosition)
{
	EnterMap(CameraPosition.x / C_TileSize, CameraPosition.z / C_TileSize);
}

void CMap::EnterMap(int32 x, int32 z)
{
	if (IsBadTileIndex(x, z) || false == m_WDT->getTileFlags(x, z).Flag_HasADT)
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

	if (false == m_WDT->getTileFlags(x, z).Flag_HasADT)
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

uint32 CMap::GetAreaID(const ICameraComponent3D* camera)
{
	if (!m_WDT->MapHasTiles())
	{
		return UINT32_MAX;
	}

	int32 tileX = (int)(camera->GetPosition().x / C_TileSize);
	int32 tileZ = (int)(camera->GetPosition().z / C_TileSize);

	int32 chunkX = (int)(fmod(camera->GetPosition().x, C_TileSize) / C_ChunkSize);
	int32 chunkZ = (int)(fmod(camera->GetPosition().z, C_TileSize) / C_ChunkSize);

	if (
		(tileX < m_CurrentTileX - static_cast<int32>(C_RenderedTiles / 2)) ||
		(tileX > m_CurrentTileX + static_cast<int32>(C_RenderedTiles / 2)) ||
		(tileZ < m_CurrentTileZ - static_cast<int32>(C_RenderedTiles / 2)) ||
		(tileZ > m_CurrentTileZ + static_cast<int32>(C_RenderedTiles / 2))
		)
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
