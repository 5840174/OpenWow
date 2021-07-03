#include "stdafx.h"

// Include
#include "Map.h"

// General
#include "MapWDL.h"

// Additional
#include "Map/MapWDLTileModel.h"
#include "WoWChunkReader.h"
#include "Sky/SkyManager.h"

CMapWDL::CMapWDL(const CMap& Map)
	: m_Minimap(nullptr)
	, m_Map(Map)
{}

CMapWDL::~CMapWDL()
{}

//--

//
// ISceneNodeProvider
//
void CMapWDL::CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const
{
	std::string fileName = m_Map.GetMapFolder() + ".wdl";

	// Low-resolution tiles
	auto f = m_Map.GetBaseManager().GetManager<IFilesManager>()->Open(fileName);
	if (f == nullptr)
		throw CException("World[%s]: WDL: Error opening.", fileName.c_str());

	// Material
	m_LowResilutionTileMaterial = std::make_shared<CMapWDLTileMaterial>(m_Map.GetBaseManager().GetApplication().GetRenderDevice());

	// Heightmap
	glm::vec3 lowres[17][17];
	glm::vec3 lowsub[16][16];

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
						lowres[y][x] = glm::vec3(C_TileSize*(i + x / 16.0f), tilebuf[y * 17 + x], C_TileSize*(j + y / 16.0f));

				for (int y = 0; y < 16; y++)
					for (int x = 0; x < 16; x++)
						lowsub[y][x] = glm::vec3(C_TileSize*(i + (x + 0.5f) / 16.0f), tilebuf2[y * 16 + x], C_TileSize*(j + (y + 0.5f) / 16.0f));


				std::vector<glm::vec3> vecrtices;

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

				// Vertex buffer
				std::shared_ptr<IBuffer> vertices = m_Map.GetBaseManager().GetApplication().GetRenderDevice().GetObjectsFactory().CreateVertexBuffer(vecrtices);

				std::shared_ptr<IGeometry> geometry = m_Map.GetBaseManager().GetApplication().GetRenderDevice().GetObjectsFactory().CreateGeometry();
				geometry->SetVertexBuffer(vertices);

				std::shared_ptr<CMapWDLTileModel> model = std::make_shared<CMapWDLTileModel>(m_Map.GetBaseManager().GetApplication().GetRenderDevice(), m_Map, i, j);
				model->AddConnection(m_LowResilutionTileMaterial, geometry);

				Parent->GetComponentT<IModelComponent>()->AddModel(model);
			}
		}
	}


	// Load low-resolution WMOs
#ifdef USE_WMO_MODELS
	Log::Green("CMapWDL: Low WMOs count [%d].", m_LowResolutionWMOsPlacementInfo.size());
	for (const auto& it : m_LowResolutionWMOsPlacementInfo)
	{
		/*if (std::shared_ptr<CWMO> wmo = m_Map.GetBaseManager().GetManager<IWoWObjectsCreator>()->LoadWMO(m_Map.GetBaseManager().GetApplication().GetRenderDevice(), m_LowResolutionWMOsNames[it.nameIndex], true))
		{
			//auto wmoInstance = Parent->CreateSceneNode<CMapWMOInstance>(wmo, it);
			//m_Map.GetBaseManager().GetManager<ILoader>()->AddToLoadQueue(wmoInstance);

			//m_LowResolutionWMOs.push_back(wmoInstance.get());
		}
		else
		{
			Log::Warn("CMapWDL: WMO model '%s' is nullptr.", m_LowResolutionWMOsNames[it.nameIndex].c_str());
		}*/
	}
#endif
}

void CMapWDL::UpdateCamera(const ICameraComponent3D * camera)
{
	if (m_LowResilutionTileMaterial)
		m_LowResilutionTileMaterial->SetDiffuseColor(glm::vec4(m_Map.GetBaseManager().GetManager<SkyManager>()->GetColor(ESkyColors::SKY_COLOR_FOG), 1.0f));
}

void CMapWDL::Load()
{
	// Offsets to MARE
	memset(m_MAREOffsets, 0x00, C_TilesInMap * C_TilesInMap * sizeof(uint32));

	std::shared_ptr<IByteBuffer> bytes = m_Map.GetBaseManager().GetManager<IFilesManager>()->Open(m_Map.GetMapFolder() + ".wdl");

	WoWChunkReader reader(m_Map.GetBaseManager(), bytes);

	if (auto buffer = reader.OpenChunk("MVER"))
	{
		uint32 version;
		buffer->readBytes(&version, 4);
		_ASSERT_EXPR(version == 18, "Version mismatch != 18");
	}

	if (auto buffer = reader.OpenChunk("MWMO")) // Filenames for WMO that appear in the low resolution map. Zero terminated strings.
	{
		PasreChunkAsStringArray(buffer, &m_LowResolutionWMOsNames);
	}

	if (auto buffer = reader.OpenChunk("MWID")) // List of indexes into the MWMO chunk.
	{
	}

#ifdef USE_WMO_MODELS
	for (const auto& placement : reader.OpenChunkT<SMapTile_MODF>("MODF")) // Placement information for the WMO. Appears to be the same 64 byte structure used in the WDT and ADT MODF chunks.
	{
		m_LowResolutionWMOsPlacementInfo.push_back(placement);
	}
#endif

	if (auto buffer = reader.OpenChunk("MAOF")) // Contains 64*64 = 4096 unsigned 32-bit integers, these are absolute offsets in the file to each map tile's MapAreaLow-array-entry. For unused tiles the value is 0.
	{
		buffer->readBytes(m_MAREOffsets, C_TilesInMap * C_TilesInMap * sizeof(uint32));
	}

	//if (auto buffer = reader.OpenChunk("MARE")) // Heightmap for one map tile.
	//{
		// Contains 17 * 17 + 16 * 16 = 545 signed 16-bit integers. So a 17 by 17 grid of height values is given, with additional height values in between grid points. Here, the "outer" 17x17 points are listed (in the usual row major order), followed by 16x16 "inner" points. The height values are on the same scale as those used in the regular height maps.
	//}

	//if (auto buffer = reader.OpenChunk("MAHO"))
	//{
		// After each MARE chunk there follows a MAHO (MapAreaHOles) chunk. It may be left out if the data is supposed to be 0 all the time. Its an array of 16 shorts. Each short is a bitmask. If the bit is not set, there is a hole at this position.
	//}

	// Minimap
	std::shared_ptr<CImageBase> mimimapImage = MakeShared(CImageBase, "", 512, 512, 32, false);

	// Heightmap
	glm::vec3 lowres[17][17];

	for (uint8 j = 0; j < C_TilesInMap; j++)
	{
		for (uint8 i = 0; i < C_TilesInMap; i++)
		{
			if (m_MAREOffsets[j][i])
			{
				// Read data             
				bytes->seek(m_MAREOffsets[j][i] + 4 + 4);

				int16 tilebuf[17 * 17];
				bytes->readBytes(tilebuf, 17 * 17 * 2);

				// make minimap
				for (int z = 0; z < 8; z++)
				{
					for (int x = 0; x < 8; x++)
					{
						short hval = tilebuf[(z * 2) * 17 + x * 2]; // for now
						uint8 r, g, b;
						if (hval < 0)
						{
							// water = blue
							if (hval < -511)
								hval = -511;
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

						mimimapImage->GetPixel<uint32>(i * 8 + x, j * 8 + z, 0) = (r) | (g << 8) | (b << 16) | (0xFF << 24);
					}
				}
			}
		}
	}

	// Finish minimap
	m_Minimap = m_Map.GetBaseManager().GetApplication().GetRenderDevice().GetObjectsFactory().CreateEmptyTexture();
	m_Minimap->LoadTexture2DFromImage(mimimapImage);
}