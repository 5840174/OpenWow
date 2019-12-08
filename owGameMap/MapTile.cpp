#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "MapTile.h"

CMapTile::CMapTile() 
{}

CMapTile::~CMapTile()
{}

void CMapTile::Initialize()
{
}

void CMapTile::Initialize(uint32 _intexX, uint32 _intexZ)
{
    m_IndexX = _intexX;
    m_IndexZ = _intexZ;

    // CTransformComponent
    std::shared_ptr<CTransformComponent3D> transformComponent = GetComponent<CTransformComponent3D>();
    {
        //transformComponent->SetTranslate(vec3(_intexX * C_TileSize, 0.0f, _intexZ * C_TileSize));
    }

    // CColliderComponent
    {
        std::shared_ptr<CColliderComponent3D> colliderComponent = GetComponent<CColliderComponent3D>();
        vec3 translate = transformComponent->GetTranslation();

        BoundingBox bbox
        (
            vec3(translate.x, Math::MaxFloat, translate.z),
            vec3(translate.x + C_TileSize, Math::MinFloat, translate.z + C_TileSize)
        );
        colliderComponent->SetBounds(bbox);
    }
}

std::shared_ptr<CMapChunk> CMapTile::getChunk(int32 x, int32 z)
{
    if (x < 0 || x >= C_ChunksInTile || z < 0 || z >= C_ChunksInTile)
        return nullptr;

    return m_Chunks[x * C_ChunksInTile + z];
}

//
// SceneNode3D
//

bool CMapTile::Accept(IVisitor* visitor)
{
	AbstractPass* visitorAsBasePass = dynamic_cast<AbstractPass*>(visitor);
	const ICamera* camera = visitorAsBasePass->GetRenderEventArgs()->Camera;

	//if (!GetMapController()->getTileIsCurrent(m_IndexX, m_IndexZ))
	//{
	//	return false;
	//}

	// Check frustrum
	//if (!CheckFrustum(camera))
	//{
	//	return false;
	//}

	return CSceneNodeProxie::Accept(visitor);
}

bool CMapTile::Load()
{
	char filename[256];
	sprintf_s(filename, "%s_%d_%d.adt", GetMapController()->GetMapFolder().c_str(), m_IndexX, m_IndexZ);

	std::shared_ptr<IFile> f = GetManager<IFilesManager>(GetBaseManager())->Open(filename);
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
		it->diffuseTexture = GetManager<IRenderDevice>(GetBaseManager())->CreateTexture2D(it->textureName);

		// PreLoad specular texture
		std::string specularTextureName = it->textureName;
		specularTextureName = specularTextureName.insert(specularTextureName.length() - 4, "_s");
		it->specularTexture = GetManager<IRenderDevice>(GetBaseManager())->CreateTexture2D(specularTextureName);
	}

	//-- Load Chunks ---------------------------------------------------------------------

	for (uint32_t i = 0; i < C_ChunksInTileGlobal; i++)
	{
		std::shared_ptr<CMapChunk> chunk = GetMapController()->CreateSceneNode<CMapChunk>(GetMapController(), std::static_pointer_cast<CMapTile>(shared_from_this()));
        chunk->Initialize(f->Path_Name(), chunks[i]);
		GetManager<ILoader>(GetBaseManager())->AddToLoadQueue(chunk);
		m_Chunks.push_back(chunk);

		// Update THIS bounds
		//BoundingBox bbox = GetBounds();
		//bbox.makeUnion(chunk->GetBounds());
		//SetBounds(bbox);
	}

	//-- WMOs --------------------------------------------------------------------------

	for (auto& it : m_WMOsPlacementInfo)
	{
//#ifndef _DEBUG
		std::shared_ptr<CMapWMOInstance> inst = CreateSceneNode<CMapWMOInstance>(m_WMOsNames[it.nameIndex]);
        inst->Initialize(it);
		GetManager<ILoader>(GetBaseManager())->AddToLoadQueue(inst);
        m_WMOsInstances.push_back(inst);
//#endif

		// Update THIS bounds
		//BoundingBox bbox = GetBounds();
		//bbox.makeUnion(inst->GetBounds());
		//SetBounds(bbox);
	}

	//-- MDXs -------------------------------------------------------------------------
#ifdef USE_M2_MODELS
	for (auto& it : m_MDXsPlacementInfo)
	{
//#ifndef _DEBUG
//		std::shared_ptr<CMapM2Instance> inst = CreateSceneNode<CMapM2Instance>(m_MDXsNames[it.nameIndex]);
//        inst->Initialize(it);
//		Application::Get().GetLoader()->AddToLoadQueue(inst);
//		m_MDXsInstances.push_back(inst);
//#endif

		// Update THIS bounds
		//BoundingBox bbox = GetBounds();
		//bbox.makeUnion(inst->GetBounds());
		//SetBounds(bbox);
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



//
// Protected
//
std::shared_ptr<CMap> CMapTile::GetMapController() const
{
    return std::dynamic_pointer_cast<CMap>(GetParent());
}
