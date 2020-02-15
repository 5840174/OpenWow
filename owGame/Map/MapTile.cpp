#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "MapTile.h"

CMapTile::CMapTile(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const CMap& Map, uint32 IndexX, uint32 IndexZ)
	: m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_Map(Map)
	, m_IndexX(IndexX)
	, m_IndexZ(IndexZ)
{}

CMapTile::~CMapTile()
{
	Log::Warn("MapTile [%d, %d] unloaded.", m_IndexX, m_IndexZ);
}


const int CMapTile::getIndexX() const
{
	return m_IndexX;
}

const int CMapTile::getIndexZ() const
{
	return m_IndexZ;
}

const CMapChunk* CMapTile::getChunk(int32 x, int32 z) const
{
    if (x < 0 || x >= C_ChunksInTile || z < 0 || z >= C_ChunksInTile)
        return nullptr;

    return m_Chunks[x * C_ChunksInTile + z];
}

const CMap & CMapTile::GetMap() const
{
	return m_Map;
}

//
// SceneNode3D
//
void CMapTile::Initialize()
{
	// CColliderComponent
	{
		std::shared_ptr<CColliderComponent3D> colliderComponent = GetComponent<CColliderComponent3D>();
		vec3 translate = GetTranslation();

		BoundingBox bbox
		(
			vec3(translate.x, Math::MaxFloat, translate.z),
			vec3(translate.x + C_TileSize, Math::MinFloat, translate.z + C_TileSize)
		);
		colliderComponent->SetBounds(bbox);
	}
}

std::string CMapTile::GetName() const
{
	return "MapTile " + std::to_string(m_IndexX) + " - " + std::to_string(m_IndexZ);
}

bool CMapTile::Load()
{
	char filename[256];
	sprintf_s(filename, "%s_%d_%d.adt", m_Map.GetMapFolder().c_str(), m_IndexX, m_IndexZ);

	std::shared_ptr<IFile> f = GetBaseManager()->GetManager<IFilesManager>()->Open(filename);
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
		f->readBytes(&header, sizeof(ADT_MHDR));
	}

	// Chunks info
	ADT_MCIN chunks[256];
	f->seek(startPos + header.MCIN);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		uint32 count = size / sizeof(ADT_MCIN);
		_ASSERT(count == C_ChunksInTileGlobal);
		memcpy(chunks, f->getDataFromCurrent(), sizeof(ADT_MCIN) * count);
	}

	// TextureInfo
	f->seek(startPos + header.MTEX);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		WOWCHUNK_READ_STRINGS_BEGIN

		std::shared_ptr<ADT_TextureInfo> textureInfo = std::make_shared<ADT_TextureInfo>();
		textureInfo->textureName = _string;

		m_Textures.push_back(textureInfo);

		WOWCHUNK_READ_STRINGS_END
	}

	// M2 names
	std::vector<std::string> m_MDXsNames;
	f->seek(startPos + header.MMDX);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		WOWCHUNK_READ_STRINGS_BEGIN
			m_MDXsNames.push_back(_string);
		WOWCHUNK_READ_STRINGS_END
		
	}

	// M2 Offsets
	std::vector<uint32> m_MDXsOffsets;
	f->seek(startPos + header.MMID);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		uint32 count = size / sizeof(uint32);
		_ASSERT(count == m_MDXsNames.size());
		for (uint32_t i = 0; i < count; i++)
		{
			uint32 offset;
			f->readBytes(&offset, sizeof(uint32));
			m_MDXsOffsets.push_back(offset);
		}
	}

	// WMO Names
	std::vector<std::string> m_WMOsNames;
	f->seek(startPos + header.MWMO);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		WOWCHUNK_READ_STRINGS_BEGIN
			m_WMOsNames.push_back(_string);
		WOWCHUNK_READ_STRINGS_END
	}

	// WMO Offsets
	std::vector<uint32> m_WMOsOffsets;
	f->seek(startPos + header.MWID);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		uint32 count = size / sizeof(uint32);
		_ASSERT(count == m_WMOsNames.size());
		for (uint32_t i = 0; i < count; i++)
		{
			uint32 offset;
			f->readBytes(&offset, sizeof(uint32));
			m_WMOsOffsets.push_back(offset);
		}
	}

	// M2 PlacementInfo
#ifdef USE_M2_MODELS
	std::vector<ADT_MDXDef> m_MDXsPlacementInfo;
	f->seek(startPos + header.MDDF);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		for (uint32 i = 0; i < size / sizeof(ADT_MDXDef); i++)
		{
			ADT_MDXDef placementInfo;
			f->readBytes(&placementInfo, sizeof(ADT_MDXDef));
			m_MDXsPlacementInfo.push_back(placementInfo);
		}
	}
#endif

	// WMO PlacementInfo
	std::vector<ADT_MODF> m_WMOsPlacementInfo;
	f->seek(startPos + header.MODF);
	{
		f->seekRelative(4);
		uint32_t size;
		f->readBytes(&size, sizeof(uint32_t));

		for (uint32 i = 0; i < size / sizeof(ADT_MODF); i++)
		{
			ADT_MODF placementInfo;
			f->readBytes(&placementInfo, sizeof(ADT_MODF));
			m_WMOsPlacementInfo.push_back(placementInfo);
		}
	}

	//-- Load Textures -------------------------------------------------------------------

	for (auto& it : m_Textures)
	{
		// PreLoad diffuse texture
		it->diffuseTexture = m_RenderDevice.GetObjectsFactory().LoadTexture2D(it->textureName);

		// PreLoad specular texture
		std::string specularTextureName = it->textureName;
		specularTextureName = specularTextureName.insert(specularTextureName.length() - 4, "_s");
		it->specularTexture = m_RenderDevice.GetObjectsFactory().LoadTexture2D(specularTextureName);
	}

	//-- Load Chunks ---------------------------------------------------------------------

	for (uint32_t i = 0; i < C_ChunksInTileGlobal; i++)
	{
		CMapChunk* chunk = CreateSceneNode<CMapChunk>(m_RenderDevice, m_Map, *this, std::make_shared<CByteBuffer>(f->getData() + chunks[i].offset, chunks[i].size));
		GetBaseManager()->GetManager<ILoader>()->AddToLoadQueue(chunk);
		m_Chunks.push_back(chunk);
	}


	//-- WMOs --------------------------------------------------------------------------

	for (auto& it : m_WMOsPlacementInfo)
	{
//#ifndef _DEBUG
		std::shared_ptr<CWMO> wmo = m_BaseManager->GetManager<IWMOManager>()->Add(m_RenderDevice, m_WMOsNames[it.nameIndex]);
		if (wmo)
		{
			CMapWMOInstance* inst = CreateSceneNode<CMapWMOInstance>(*wmo, it);
			GetBaseManager()->GetManager<ILoader>()->AddToLoadQueue(inst);
			m_WMOsInstances.push_back(inst);
		}
//#endif
	}


	//-- MDXs -------------------------------------------------------------------------
#ifdef USE_M2_MODELS
	for (auto& it : m_MDXsPlacementInfo)
	{
//#ifndef _DEBUG
		std::shared_ptr<M2> m2 = m_BaseManager->GetManager<IM2Manager>()->Add(m_RenderDevice, m_MDXsNames[it.nameIndex]);
		if (m2)
		{
			CMapM2Instance* inst = CreateSceneNode<CMapM2Instance>(*m2, it);
			GetBaseManager()->GetManager<ILoader>()->AddToLoadQueue(inst);
			m_MDXsInstances.push_back(inst);
		}
//#endif
	}
	//---------------------------------------------------------------------------------
#endif

	Log::Green("CMapTile[%d, %d, %s]: Loaded!", m_IndexX, m_IndexZ, filename);

	return true;
}

bool CMapTile::Delete()
{
	return true;
}
