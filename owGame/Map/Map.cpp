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

CMap::CMap(IScene& Scene, IBaseManager& BaseManager, IRenderDevice& RenderDevice)
	: CSceneNode(Scene)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
{
	m_CurrentTileX = m_CurrentTileZ = -1;
	m_IsOnInvalidTile = false;

	if (_MapShared == nullptr)
	{
		_MapShared = new CMapShared(m_RenderDevice);
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

void CMap::MapPreLoad(const DBC_MapRecord* _map)
{
	m_MapDBCRecord = _map;
	m_MapFolderName = CMapShared::getMapFolder(m_MapDBCRecord);

	Log::Print("Map[%s]: Id [%d]. Preloading...", m_MapDBCRecord->Get_Directory(), m_MapDBCRecord->Get_ID());

	m_WDL = std::make_unique<CMapWDL>(m_BaseManager, m_RenderDevice, *this);
	m_WDL->Load();

	m_WDT = std::make_unique<CMapWDT>(m_BaseManager, m_RenderDevice, *this);
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
		if (m_ADTCache[i] != nullptr)
		{
			RemoveChild(m_ADTCache[i]);
		}
	}

	for (int i = 0; i < C_RenderedTiles; i++)
	{
		for (int j = 0; j < C_RenderedTiles; j++)
		{
			RemoveChild(m_Current[i][j]);
		}
	}
}

// --

void CMap::Update(const UpdateEventArgs& e)
{
	const ICameraComponent3D* camera = e.CameraForCulling;

	bool loading = false;
	int enteredTileX, enteredTileZ;
	int midTile = static_cast<uint32>(C_RenderedTiles / 2);
	if (m_Current[midTile][midTile] != nullptr || m_IsOnInvalidTile)
	{
		if (m_IsOnInvalidTile ||
			(camera->GetPosition().x < m_Current[midTile][midTile]->GetPosition().x) ||
			(camera->GetPosition().x > (m_Current[midTile][midTile]->GetPosition().x + C_TileSize)) ||
			(camera->GetPosition().z < m_Current[midTile][midTile]->GetPosition().z) ||
			(camera->GetPosition().z > (m_Current[midTile][midTile]->GetPosition().z + C_TileSize)))
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

void CMap::EnterMap(glm::vec3 _cameraPosition)
{
	EnterMap(_cameraPosition.x / C_TileSize, _cameraPosition.z / C_TileSize);
}

void CMap::EnterMap(int32 x, int32 z)
{
	if (IsBadTileIndex(x, z) || !m_WDT->getTileFlags(x, z).Flag_HasADT)
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
			m_Current[i][j] = LoadTile(x - static_cast<uint32>(C_RenderedTiles / 2) + i, z - static_cast<uint32>(C_RenderedTiles / 2) + j);
		}
	}
}

std::shared_ptr<CMapTile> CMap::LoadTile(int32 x, int32 z)
{
	if (IsBadTileIndex(x, z))
	{
		return nullptr;
	}

	if (!m_WDT->getTileFlags(x, z).Flag_HasADT)
	{
		return nullptr;
	}

	// Try get tile from cache
	int firstnull = C_TilesCacheSize;
	for (int i = 0; i < C_TilesCacheSize; i++)
	{
		if ((m_ADTCache[i] != nullptr) && (m_ADTCache[i]->getIndexX() == x) && (m_ADTCache[i]->getIndexZ() == z))
		{
			return m_ADTCache[i];
		}

		if ((m_ADTCache[i] == nullptr) && (i < firstnull))
		{
			firstnull = i;
		}
	}

	// ok we need to find a place in the cache
	if (firstnull == C_TilesCacheSize)
	{
		int score, maxscore = 0, maxidx = 0;
		// oh shit we need to throw away a tile
		for (int i = 0; i < C_TilesCacheSize; i++)
		{
			if (m_ADTCache[i] == nullptr)
			{
				continue;
			}

			score = abs(m_ADTCache[i]->getIndexX() - m_CurrentTileX) + abs(m_ADTCache[i]->getIndexZ() - m_CurrentTileZ);

			if (score > maxscore)
			{
				maxscore = score;
				maxidx = i;
			}
		}

		// maxidx is the winner (loser)
		RemoveChild(m_ADTCache[maxidx]);
		m_BaseManager.GetManager<ILoader>()->AddToDeleteQueue(m_ADTCache[maxidx]);
		firstnull = maxidx;
	}

	// Create new tile
	m_ADTCache[firstnull] = CreateSceneNode<CMapTile>(m_BaseManager, m_RenderDevice, *this, x, z);
	m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(m_ADTCache[firstnull]);

	return m_ADTCache[firstnull];
}

void CMap::ClearCache()
{
	for (int i = 0; i < C_TilesCacheSize; i++)
	{
		if (m_ADTCache[i] != nullptr && !IsTileInCurrent(*m_ADTCache[i]))
		{
			m_ADTCache[i] = 0;
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

	auto curTile = m_Current[indexX][indexY];
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
    auto currentTile = m_Current[midTile][midTile];
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
			if (m_Current[i][j] != nullptr)
				if (m_Current[i][j].get() == &_mapTile)
					return true;
	return false;
}
