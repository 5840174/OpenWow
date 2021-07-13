#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "MapTile.h"

// Additional
#include "MapTileLiquid.h"

CMapTile::CMapTile(IScene& Scene, CMap& MapParent, uint32 IndexX, uint32 IndexZ)
	: CSceneNode(Scene)
	, m_MapParent(MapParent)
	, m_IndexX(IndexX)
	, m_IndexZ(IndexZ)
{
	SetName("MapTile[" + std::to_string(getIndexX()) + "," + std::to_string(getIndexZ()) + "]");
}

CMapTile::~CMapTile()
{
	//Log::Warn("CMapTile::~CMapTile: Tile [%d, %d] unloaded.", m_IndexX, m_IndexZ);
}

CMap& CMapTile::GetMap() const
{
	return m_MapParent;
}

int CMapTile::getIndexX() const
{
	return m_IndexX;
}

int CMapTile::getIndexZ() const
{
	return m_IndexZ;
}

const CMapChunk* CMapTile::getChunk(int32 x, int32 z) const
{
    if (x < 0 || x >= C_ChunksInTile || z < 0 || z >= C_ChunksInTile)
        return nullptr;

    return m_Chunks[x * C_ChunksInTile + z];
}

bool CMapTile::IsNortrend() const
{
	return m_Header.flags.IsNortrend;
}

std::shared_ptr<SMapTile_MTEX> CMapTile::GetTextureInfo(size_t Index) const
{
	if (Index >= m_Textures.size())
		return nullptr;
	return m_Textures[Index];
}

void CMapTile::ExtendMapTileBounds(const BoundingBox & OtherBBox)
{
	ExtendBounds(OtherBBox);
}



//
// ISceneNode
//
void CMapTile::Initialize()
{
	__super::Initialize();

	// Do not set local transform
	//SetLocalPosition(glm::vec3(m_IndexX * C_TileSize, 0.0f, m_IndexZ * C_TileSize));

	// Collider
	BoundingBox bbox
	(
		glm::vec3((getIndexX() * C_TileSize),              Math::MaxFloat, (getIndexZ() * C_TileSize)),
		glm::vec3((getIndexX() * C_TileSize) + C_TileSize, Math::MinFloat, (getIndexZ() * C_TileSize) + C_TileSize)
	);

	SetBounds(bbox);
	SetCullStrategy(ECullStrategy::ByFrustrumAndDistance2D);
	SetCullDistance(GetBaseManager().GetManager<ISettings>()->GetGroup("WoWSettings")->GetPropertyT<float>("MapChunkRenderDistance")->Get());
	SetDebugDrawMode(false);
	SetDebugDrawColor(ColorRGBA(0.5f, 0.8f, 0.2f, 0.8f));
}



//
// ILoadableObject
//
bool CMapTile::Load()
{
	std::string mapTileFilename = GetMap().GetMapFolder() + "_" + std::to_string(m_IndexX) + "_" + std::to_string(m_IndexZ) + ".adt";

	auto f = GetBaseManager().GetManager<IFilesManager>()->Open(mapTileFilename);
	if (f == nullptr)
		//throw CException("CMapTile::Load: Unable to load tile. Filename = '%s'. IndexX = '%d'. IndexZ = '%d'.", mapTileFilename.c_str(), m_IndexX, m_IndexZ);
		return false;

	uint32_t startPos = f->getPos() + 20;
	
	// MVER + size (8)
	f->seekRelative(8);
	{
		uint32 version;
		f->readBytes(&version, 4);
		_ASSERT(version == 18);
	}

	// MHDR + size (8)
	f->seekRelative(8);
	{
		f->readBytes(&m_Header, sizeof(SMapTile_MHDR));
	}

	// Chunks info
	SMapTile_MCIN chunks[C_ChunksInTileGlobal];
	f->seek(startPos + m_Header.MCIN);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		uint32 count = size / sizeof(SMapTile_MCIN);
		_ASSERT(count == C_ChunksInTileGlobal);
		memcpy(chunks, f->getDataFromCurrent(), sizeof(SMapTile_MCIN) * count);
	}

	// Liquids
	if (m_Header.MH20 != 0)
	{
		f->seek(startPos + m_Header.MH20);
		{
			f->seekRelative(4);
			uint32_t size;
			f->readBytes(&size, sizeof(uint32_t));
			_ASSERT(size > 0);

			const uint8* abuf = f->getDataFromCurrent();
			for (uint32 i = 0; i < C_ChunksInTile; i++)
			{
				for (uint32 j = 0; j < C_ChunksInTile; j++)
				{
					MH2O_Header* mh2o_Header = (MH2O_Header*)abuf;
					if (mh2o_Header->layersCount > 0)
					{
						CMapTileLiquid liquidObject(GetRenderDevice(), f, *mh2o_Header);

						auto liquidInstance = CreateSceneNode<CLiquidBaseInstance>();
						liquidObject.CreateInsances(liquidInstance);

						// Transform
						liquidInstance->SetLocalPosition(glm::vec3((getIndexX() * C_TileSize) + (j * C_ChunkSize), 0.0f, (getIndexZ() * C_TileSize) + (i * C_ChunkSize)));

						// Collider
						liquidInstance->SetBounds(BoundingBox(
							glm::vec3(0.0f,        liquidObject.getMinHeight() - 1.0f, 0.0f),
							glm::vec3(C_ChunkSize, liquidObject.getMaxHeight() + 1.0f, C_ChunkSize)
						));

						ExtendMapTileBounds(liquidInstance->GetWorldBounds());
					}
					abuf += sizeof(MH2O_Header);
				}
			}
		}
	}

	// TextureInfo
	f->seek(startPos + m_Header.MTEX);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		if (size > 0)
		{
			std::vector<std::string> strings;
			PasreChunkAsStringArray(MakeShared(CByteBufferOnlyPointer, f->getDataFromCurrent(), size), &strings);
			//Log::Info("Textures count = '%d'.", strings.size());

			uint32 textureSizeX = -1;
			uint32 textureSizeY = -1;

			for (const auto& stringsIt : strings)
			{

				auto textureInfo = MakeShared(SMapTile_MTEX);
				textureInfo->textureName = stringsIt;

				// PreLoad diffuse texture
#if 0
				textureInfo->diffuseTexture = GetBaseManager().GetManager<IImagesFactory>()->CreateImage(_string);
#else
				textureInfo->diffuseTexture = GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(textureInfo->textureName);
#endif
				/*if (textureSizeX == -1)
					textureSizeX = textureInfo->diffuseTexture->GetWidth();
				if (textureSizeX != textureInfo->diffuseTexture->GetWidth())
					throw CException("TextureX size error. Stored '%d'. New '%d'.", textureSizeX, textureInfo->diffuseTexture->GetWidth());

				if (textureSizeY == -1)
					textureSizeY = textureInfo->diffuseTexture->GetHeight();
				if (textureSizeY != textureInfo->diffuseTexture->GetHeight())
					throw CException("TextureY size error. Stored '%d'. New '%d'.", textureSizeY, textureInfo->diffuseTexture->GetHeight());*/


				// PreLoad specular texture
				try
				{
					std::string specularTextureName = textureInfo->textureName;
					specularTextureName = specularTextureName.insert(specularTextureName.length() - 4, "_s");
#if 0
					textureInfo->specularTexture = GetBaseManager().GetManager<IImagesFactory>()->CreateImage(specularTextureName);
#else
					textureInfo->specularTexture = GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(specularTextureName);
#endif
				}
				catch (const CException& e)
				{
				}

				m_Textures.push_back(textureInfo);

			}

			m_ArrayTexture = GetRenderDevice().GetObjectsFactory().CreateEmptyTexture();
			//m_ArrayTexture->LoadTexture2DArrayFromBytes();
		}
	}

	// M2 names
	std::vector<std::string> m_MDXsNames;
	f->seek(startPos + m_Header.MMDX);
	{
		f->seekRelative(4);
		uint32 size;
		f->readBytes(&size, sizeof(uint32));

		if (size > 0)
			PasreChunkAsStringArray(MakeShared(CByteBufferOnlyPointer, f->getDataFromCurrent(), size), &m_MDXsNames);
	}

	// M2 Offsets
	std::vector<uint32> m_MDXsOffsets;
	f->seek(startPos + m_Header.MMID);
	{
		f->seekRelative(4);
		uint32 size;
		f->readBytes(&size, sizeof(uint32));

		uint32 count = size / sizeof(uint32);
		_ASSERT(count == m_MDXsNames.size());
		for (uint32 i = 0; i < count; i++)
		{
			uint32 offset;
			f->readBytes(&offset, sizeof(uint32));
			m_MDXsOffsets.push_back(offset);
		}
	}

	// WMO Names
	std::vector<std::string> WMOsNames;
	f->seek(startPos + m_Header.MWMO);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32));

		if (size > 0)
			PasreChunkAsStringArray(MakeShared(CByteBufferOnlyPointer, f->getDataFromCurrent(), size), &WMOsNames);
	}

	// WMO Offsets
	std::vector<uint32> WMOsOffsets;
	f->seek(startPos + m_Header.MWID);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32));
		uint32 count = size / sizeof(uint32);

		_ASSERT(count == WMOsNames.size());

		for (uint32_t i = 0; i < count; i++)
		{
			uint32 offset;
			f->readBytes(&offset, sizeof(uint32));
			WMOsOffsets.push_back(offset);
		}
	}

	// M2 PlacementInfo
#ifdef USE_M2_MODELS
	std::vector<SMapTile_MDDF> M2PlacementInfo;
	f->seek(startPos + m_Header.MDDF);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		for (uint32 i = 0; i < size / sizeof(SMapTile_MDDF); i++)
		{
			SMapTile_MDDF placementInfo;
			f->readBytes(&placementInfo, sizeof(SMapTile_MDDF));
			M2PlacementInfo.push_back(placementInfo);
		}
	}
#endif

	// WMO PlacementInfo
#ifdef USE_WMO_MODELS
	std::vector<SMapTile_MODF> WMOPlacementInfo;
	f->seek(startPos + m_Header.MODF);
	{
		f->seekRelative(4);

		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		for (uint32 i = 0; i < size / sizeof(SMapTile_MODF); i++)
		{
			SMapTile_MODF placementInfo;
			f->readBytes(&placementInfo, sizeof(SMapTile_MODF));
			WMOPlacementInfo.push_back(placementInfo);
		}
	}
#endif

	//SetState(ILoadable::ELoadableState::Loaded);

	//-- Load Chunks ---------------------------------------------------------------------
	for (uint32_t i = 0; i < C_ChunksInTileGlobal; i++)
	{
		auto chunk = CreateSceneNode<CMapChunk>(*this, chunks[i], f);
		chunk->AddParentLoadable(std::dynamic_pointer_cast<ILoadable>(shared_from_this()));
		AddChildLoadable(chunk);
		GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(chunk);

		m_Chunks.push_back(chunk.get());
	}


	//-- WMOs --------------------------------------------------------------------------
#ifdef USE_WMO_MODELS
	for (const auto& it : WMOPlacementInfo)
	{
		if (auto wmo = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadWMO(GetRenderDevice(), WMOsNames[it.nameIndex]))
		{
			auto inst = CreateSceneNode<CMapWMOInstance>(wmo, it);
			GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(inst);

			m_WMOsInstances.push_back(inst.get());
		}
		else
		{
			Log::Warn("CMapTile: WMO model '%s' is nullptr.", WMOsNames[it.nameIndex].c_str());
		}
	}
#endif

	//-- MDXs -------------------------------------------------------------------------
#ifdef USE_M2_MODELS
	for (const auto& it : M2PlacementInfo)
	{
		if (auto m2 = GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadM2(GetRenderDevice(), m_MDXsNames[it.nameIndex]))
		{
			auto inst = CreateSceneNode<CMapM2Instance>(m2, it);
			GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(inst);
			
			m_MDXsInstances.push_back(inst.get());
		}
		else
		{
			Log::Warn("CMapTile: M2 model '%s' is nullptr.", m_MDXsNames[it.nameIndex].c_str());
		}
	}
	//---------------------------------------------------------------------------------
#endif

	//Log::Green("MapTile[%d, %d, %s]: Loaded!", m_IndexX, m_IndexZ, mapTileFilename.c_str());

	return true;
}

bool CMapTile::Delete()
{
	return true;
}
