#include "stdafx.h"

// Include
#include "Map.h"
#include "WDL_Node_Material.h"

// General
#include "MapWDL.h"

CMapWDL::CMapWDL(IBaseManager* BaseManager, std::weak_ptr<const CMap> MapController) 
	: m_MapController(MapController)
	, m_Minimap(nullptr)
	, m_BaseManager(BaseManager)
{}

CMapWDL::~CMapWDL()
{
}

//--

//
// ISceneNodeProvider
//
void CMapWDL::CreateInsances(std::weak_ptr<ISceneNode> _parent)
{
	std::shared_ptr<const CMap> mapController = m_MapController.lock();
	_ASSERT(mapController != NULL);

	std::string fileName = mapController->GetMapFolder() + ".wdl";

	// Low-resolution tiles
	std::shared_ptr<IFile> f = m_BaseManager->GetManager<IFilesManager>()->Open(fileName);
	if (f == nullptr)
	{
		Log::Error("World[%s]: WDL: Error opening.", fileName.c_str());
		return;
	}

	// Material
	m_LowResilutionTileMaterial = std::make_shared<WDL_Node_Material>(m_BaseManager->GetManager<IRenderDevice>());
	m_LowResilutionTileMaterial->SetWrapper(m_LowResilutionTileMaterial);

	// Heightmap
	vec3 lowres[17][17];
	vec3 lowsub[16][16];

	for (uint8 j = 0; j < C_TilesInMap; j++)
	{
		for (uint8 i = 0; i < C_TilesInMap; i++)
		{
			if (m_MAREOffsets[j][i])
			{
				// Read data             
				f->seek(m_MAREOffsets[j][i] + 4 + 4);

				int16 tilebuf[17 * 17];
				f->readBytes(tilebuf, 17 * 17 * 2);

				int16 tilebuf2[16 * 16];
				f->readBytes(tilebuf2, 16 * 16 * 2);

				for (int y = 0; y < 17; y++)
					for (int x = 0; x < 17; x++)
						lowres[y][x] = vec3(C_TileSize*(i + x / 16.0f), tilebuf[y * 17 + x], C_TileSize*(j + y / 16.0f));

				for (int y = 0; y < 16; y++)
					for (int x = 0; x < 16; x++)
						lowsub[y][x] = vec3(C_TileSize*(i + (x + 0.5f) / 16.0f), tilebuf2[y * 16 + x], C_TileSize*(j + (y + 0.5f) / 16.0f));


				std::vector<vec3> vecrtices;

				for (int y = 0; y < 16; y++)
				{
					for (int x = 0; x < 16; x++)
					{
						vecrtices.push_back(lowres[y][x]);
						vecrtices.push_back(lowsub[y][x]);
						vecrtices.push_back(lowres[y][x + 1]);

						vecrtices.push_back(lowres[y][x + 1]);
						vecrtices.push_back(lowsub[y][x]);
						vecrtices.push_back(lowres[y + 1][x + 1]);

						vecrtices.push_back(lowres[y + 1][x + 1]);
						vecrtices.push_back(lowsub[y][x]);
						vecrtices.push_back(lowres[y + 1][x]);

						vecrtices.push_back(lowres[y + 1][x]);
						vecrtices.push_back(lowsub[y][x]);
						vecrtices.push_back(lowres[y][x]);
					}
				}
#if 0
				// Vertex buffer
				std::shared_ptr<IBuffer> __vb = m_BaseManager->GetManager<IRenderDevice>()->CreateVertexBuffer(vecrtices);

				std::shared_ptr<IModel> __geom = m_BaseManager->GetManager<IRenderDevice>()->CreateMesh();
				__geom->AddVertexBuffer(BufferBinding("POSITION", 0), __vb);
				__geom->SetMaterial(m_LowResilutionTileMaterial);
				
				std::shared_ptr<CWDL_LowResTile> lowResTile = std::make_shared<CWDL_LowResTile>(m_MapController, __geom, i, j);
				_parent.lock()->GetComponent<CMeshComponent3D>()->AddMesh(lowResTile);
#endif
			}
		}
	}

	// Load low-resolution WMOs
	Log::Green("Map_GlobalWMOs[]: Low WMOs count [%d].", m_LowResolutionWMOsPlacementInfo.size());
	for (auto it : m_LowResolutionWMOsPlacementInfo)
	{
		std::shared_ptr<CMapWMOInstance> wmoInstance = _parent.lock()->CreateWrappedSceneNode<CMapWMOInstance>("SceneNode3D", m_LowResolutionWMOsNames[it.nameIndex]);
		wmoInstance->Initialize(it);
		m_BaseManager->GetManager<ILoader>()->AddToLoadQueue(wmoInstance);
		m_LowResolutionWMOs.push_back(wmoInstance);
	}
}

void CMapWDL::UpdateCamera(const ICameraComponent3D * camera)
{
	//if (m_LowResilutionTileMaterial)
	//	m_LowResilutionTileMaterial->SetDiffuseColor(vec4(GetManager<ISkyManager>()->GetColor(LightColors::LIGHT_COLOR_FOG), 1.0f));
}

void CMapWDL::Load()
{
	std::shared_ptr<const CMap> mapController = m_MapController.lock();
	_ASSERT(mapController != NULL);

	std::string fileName = mapController->GetMapFolder() + ".wdl";

	std::shared_ptr<IFile> f = m_BaseManager->GetManager<IFilesManager>()->Open(fileName);
	if (f == nullptr)
	{
		Log::Info("World[%s]: WDL: Error opening.", fileName.c_str());
		return;
	}

	// Offsets to MARE
	memset(m_MAREOffsets, 0x00, C_TilesInMap * C_TilesInMap * sizeof(uint32));

	char fourcc[5];
	uint32 size;

	while (!f->isEof())
	{
		memset(fourcc, 0, 4);
		size = 0;
		f->readBytes(fourcc, 4);
		f->readBytes(&size, 4);
		flipcc(fourcc);
		fourcc[4] = 0;
		if (size == 0) continue;
		uint32_t nextpos = f->getPos() + size;

		if (strcmp(fourcc, "MVER") == 0)
		{
			uint32 version;
			f->readBytes(&version, 4);
			_ASSERT_EXPR(version == 18, "Version mismatch != 18");
		}
		else if (strncmp(fourcc, "MWMO", 4) == 0) // Filenames for WMO that appear in the low resolution map. Zero terminated strings.
		{
			WOWCHUNK_READ_STRINGS_BEGIN
				m_LowResolutionWMOsNames.push_back(_string);
			WOWCHUNK_READ_STRINGS_END;
		}
		else if (strncmp(fourcc, "MWID", 4) == 0) // List of indexes into the MWMO chunk.
		{
		}
		else if (strncmp(fourcc, "MODF", 4) == 0) // Placement information for the WMO. Appears to be the same 64 byte structure used in the WDT and ADT MODF chunks.
		{
			for (uint32 i = 0; i < size / sizeof(ADT_MODF); i++)
			{
				ADT_MODF placement;
				f->readBytes(&placement, sizeof(ADT_MODF));
				m_LowResolutionWMOsPlacementInfo.push_back(placement);
			}
		}
		else if (strncmp(fourcc, "MAOF", 4) == 0) // Contains 64*64 = 4096 unsigned 32-bit integers, these are absolute offsets in the file to each map tile's MapAreaLow-array-entry. For unused tiles the value is 0.
		{
			f->readBytes(m_MAREOffsets, C_TilesInMap * C_TilesInMap * sizeof(uint32));
		}
		else if (strncmp(fourcc, "MARE", 4) == 0) // Heightmap for one map tile.
		{
			// Contains 17 * 17 + 16 * 16 = 545 signed 16-bit integers. So a 17 by 17 grid of height values is given, with additional height values in between grid points. Here, the "outer" 17x17 points are listed (in the usual row major order), followed by 16x16 "inner" points. The height values are on the same scale as those used in the regular height maps.
		}
		else if (strncmp(fourcc, "MAHO", 4) == 0)
		{
			// After each MARE chunk there follows a MAHO (MapAreaHOles) chunk. It may be left out if the data is supposed to be 0 all the time. Its an array of 16 shorts. Each short is a bitmask. If the bit is not set, there is a hole at this position.
		}
		else
		{
			Log::Fatal("Map[%s]: WDL: Chunks [%s], Size [%d] not implemented.", fileName.c_str(), fourcc, size);
		}
		f->seek(nextpos);
	}

	// Minimap
	uint32* texbuf = new uint32[512 * 512];
	memset(texbuf, 0, 512 * 512 * 4);

	// Heightmap
	vec3 lowres[17][17];

	for (uint8 j = 0; j < C_TilesInMap; j++)
	{
		for (uint8 i = 0; i < C_TilesInMap; i++)
		{
			if (m_MAREOffsets[j][i])
			{
				// Read data             
				f->seek(m_MAREOffsets[j][i] + 4 + 4);

				int16 tilebuf[17 * 17];
				f->readBytes(tilebuf, 17 * 17 * 2);

				// make minimap
				// for a 512x512 minimap texture, and 64x64 tiles, one tile is 8x8 pixels
				for (int z = 0; z < 8; z++)
				{
					for (int x = 0; x < 8; x++)
					{
						short hval = tilebuf[(z * 2) * 17 + x * 2]; // for now
						uint8 r, g, b;
						if (hval < 0)
						{
							// water = blue
							if (hval < -511) hval = -511;
							hval /= -2;
							r = g = 0;
							b = 255 - hval;
						}
						else
						{
							uint8 r1, r2, g1, g2, b1, b2;
							float t;

							if (hval < 600)
							{
								r1 = 20;
								r2 = 137;
								g1 = 149;
								g2 = 84;
								b1 = 7;
								b2 = 21;
								t = hval / 600.0f;
							}
							else if (hval < 1200)
							{
								r2 = 96;
								r1 = 137;
								g2 = 96;
								g1 = 84;
								b2 = 96;
								b1 = 21;
								t = (hval - 600) / 600.0f;
							}
							else /*if (hval < 1600)*/
							{
								r1 = 96;
								r2 = 255;
								g1 = 96;
								g2 = 255;
								b1 = 96;
								b2 = 255;
								if (hval >= 1600) hval = 1599;
								t = (hval - 1200) / 600.0f;
							}

							// TODO: add a regular palette here

							r = (uint8)(r2*t + r1 * (1.0f - t));
							g = (uint8)(g2*t + g1 * (1.0f - t));
							b = (uint8)(b2*t + b1 * (1.0f - t));
						}

						texbuf[(j * 8 + z) * 512 + i * 8 + x] = (r) | (g << 8) | (b << 16) | (255 << 24);
					}
				}
			}
		}
	}

	// Finish minimap
	m_Minimap = m_BaseManager->GetManager<IRenderDevice>()->CreateTexture();
	m_Minimap->LoadTextureCustom(512, 512, texbuf);
	delete[] texbuf;
}