#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Group_Instance.h"

// General
#include "WMO_Group.h"

// Additional 
#include "WoWChunkReader.h"
#include "DBC/DBC__Storage.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"
#include "WMO_Fixes.h"

WMO_Group::WMO_Group(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const CWMO& WMOModel, const uint32 GroupIndex, const SWMO_GroupInfoDef& GroupProto)
	: m_IsMOCVExists(false)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_WMOModel(WMOModel)
	, m_GroupIndex(GroupIndex)
{
	if (GroupProto.nameoffset != -1)
		m_GroupName = std::string(WMOModel.m_GroupNames.get() + GroupProto.nameoffset);
	else
		m_GroupName = WMOModel.getFilename() + "_Group" + std::to_string(GroupIndex);

	m_Bounds = GroupProto.bounding_box.Convert();
}

WMO_Group::~WMO_Group()
{
}

void WMO_Group::CreateInsances(CWMO_Group_Instance* _parent) const
{
	for (const auto& batch : m_WMOBatchIndexes)
	{
		_parent->GetComponent<CMeshComponent3D>()->AddMesh(batch);
	}

	if (m_WMOLiqiud != nullptr)
	{
		vec3 realPos = Fix_XZmY(m_LiquidHeader.pos);
		realPos.y = 0.0f; // why they do this???

		CWMO_Liquid_Instance* liquid = _parent->CreateSceneNode<CWMO_Liquid_Instance>(*this);
		liquid->LiquidInitialize(m_WMOLiqiud, realPos);
		_parent->addLiquidInstance(liquid);
	}

#ifdef USE_M2_MODELS
	for (const auto& index : m_DoodadsPlacementIndexes)
	{
		const SWMO_Doodad_PlacementInfo& placement = m_WMOModel.m_DoodadsPlacementInfos[index];

		std::string doodadFileName = m_WMOModel.m_DoodadsFilenames + placement.flags.nameIndex;

		std::shared_ptr<M2> m2 = m_BaseManager->GetManager<IM2Manager>()->Add(m_RenderDevice, doodadFileName);
		if (m2)
		{
			CWMO_Doodad_Instance* inst = _parent->CreateSceneNode<CWMO_Doodad_Instance>(*m2, *this, index, placement);
			m_BaseManager->GetManager<ILoader>()->AddToLoadQueue(inst);
			_parent->addDoodadInstance(inst);
		}
	}
#endif
}

uint32 WMO_Group::to_wmo_liquid(int x)
{
	DBC_LIQUIDTYPE_Type::List basic = (DBC_LIQUIDTYPE_Type::List)(x & 3);
	switch (basic)
	{
	case DBC_LIQUIDTYPE_Type::water:
		return (m_Header.flags.IS_NOT_WATER_BUT_OCEAN) ? 14 : 13;
	case DBC_LIQUIDTYPE_Type::ocean:
		return 14;
	case DBC_LIQUIDTYPE_Type::magma:
		return 19;
	case DBC_LIQUIDTYPE_Type::slime:
		return 20;
	}

	return UINT32_MAX;
}

void WMO_Group::Load()
{
	// Buffer
	uint16* dataFromMOVI = nullptr;
	std::shared_ptr<IBuffer>	IB_Default = nullptr;
	dataFromMOVT = nullptr;
	std::shared_ptr<IBuffer>	VB_Vertexes = nullptr;
	std::vector<std::shared_ptr<IBuffer>>	VB_TextureCoords;
	std::shared_ptr<IBuffer>	VB_Normals = nullptr;
	std::shared_ptr<IBuffer>	VB_Colors = nullptr;

	// CollisionTEMP
	uint32 collisionCount = 0;
	SWMO_Group_MOBNDef* collisions = nullptr;


	char temp[MAX_PATH];
	strcpy_s(temp, m_WMOModel.getFilename().c_str());
	temp[m_WMOModel.getFilename().length() - 4] = 0;

	char groupFilename[MAX_PATH];
	sprintf_s(groupFilename, "%s_%03d.wmo", temp, m_GroupIndex);

	std::unique_ptr<WoWChunkReader> chunkReader = std::make_unique<WoWChunkReader>(m_BaseManager, groupFilename);

	// Version
	if (auto buffer = chunkReader->OpenChunk("MVER"))
	{
		uint32 version;
		buffer->readBytes(&version, 4);
		_ASSERT(version == 17);
	}


	// Header
	if (auto buffer = chunkReader->OpenChunk("MOGP"))
	{
		buffer->readBytes(&m_Header, sizeof(SWMO_Group_HeaderDef));
		_ASSERT(m_Header.flags.HAS_3_MOTV == 0);

		// Real wmo group file contains only 2 chunks: MVER and MOGP.
		// Start of MOGP is header (without fourcc).
		// After header data places others chunks.
		// We reinitialize chunk reader from current position
		// chunkReader.reset() DON'T call this, because source buffer will be free.
		chunkReader = std::make_unique<WoWChunkReader>(m_BaseManager, buffer->getDataFromCurrent(), buffer->getSize() - sizeof(SWMO_Group_HeaderDef));
	}


	// Material info for triangles
	for (const auto& materialsInfo : chunkReader->OpenChunkT<SWMO_Group_MaterialDef>("MOPY")) // Material info for triangles
	{
		m_MaterialsInfo.push_back(materialsInfo);
	}


	// Indices
	if (auto buffer = chunkReader->OpenChunk("MOVI"))
	{
		_ASSERT(IB_Default == nullptr);

		// Buffer
		IB_Default = m_RenderDevice.GetObjectsFactory().CreateIndexBuffer((const uint16*)buffer->getData(), buffer->getSize() / sizeof(uint16));
	}


	// Vertices chunk.
	if (auto buffer = chunkReader->OpenChunk("MOVT"))
	{
		_ASSERT(VB_Vertexes == nullptr);

		uint32 vertexesCount = buffer->getSize() / sizeof(vec3);
		glm::vec3* vertexes = (glm::vec3*)buffer->getData();

		// Convert
		for (uint32 i = 0; i < vertexesCount; i++)
			vertexes[i] = Fix_XZmY(vertexes[i]);

		// Buffer
		VB_Vertexes = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(vertexes, vertexesCount);
		VB_Colors = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(vertexes, vertexesCount);

		dataFromMOVT = vertexes;
	}


	// Normals
	if (auto buffer = chunkReader->OpenChunk("MONR"))
	{
		_ASSERT(VB_Normals == nullptr);

		uint32 normalsCount = buffer->getSize() / sizeof(glm::vec3);
		glm::vec3* normals = (glm::vec3*)buffer->getDataFromCurrent();

		// Convert
		for (uint32 i = 0; i < normalsCount; i++)
			normals[i] = Fix_XZmY(normals[i]);

		// Buffer
		VB_Normals = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(normals, normalsCount);
	}


	// Texture coords
	if (auto buffer = chunkReader->OpenChunk("MOTV"))
	{
		uint32 textureCoordsCount = buffer->getSize() / sizeof(vec2);
		vec2* textureCoords = (vec2*)buffer->getDataFromCurrent();

		VB_TextureCoords.push_back(m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(textureCoords, textureCoordsCount));
		VB_TextureCoords.push_back(m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(textureCoords, textureCoordsCount));
	}


	// WMO_Group_Batch
	if (auto buffer = chunkReader->OpenChunk("MOBA"))
	{
		// Temp code
		uint32 batchesCount = buffer->getSize() / sizeof(SWMO_Group_BatchDef);
		SWMO_Group_BatchDef* batches = (SWMO_Group_BatchDef*)buffer->getDataFromCurrent();

		moba = new SWMO_Group_BatchDef[batchesCount];
		memcpy(moba, batches, sizeof(SWMO_Group_BatchDef) * batchesCount);

		for (uint32 i = 0; i < buffer->getSize() / sizeof(SWMO_Group_BatchDef); i++)
		{
			SWMO_Group_BatchDef batchDef;
			buffer->readBytes(&batchDef, sizeof(SWMO_Group_BatchDef));
			m_WMOBatchs.push_back(batchDef);
		}
	}


	// Light references
	if (auto buffer = chunkReader->OpenChunk("MOLR")) 
	{
		_ASSERT(m_Header.flags.HAS_LIGHTS);
		uint32 lightsIndexesCount = buffer->getSize() / sizeof(uint16);
		uint16* lightsIndexes = (uint16*)buffer->getDataFromCurrent();
		for (uint32 i = 0; i < lightsIndexesCount; i++)
		{
			m_WMOLightsIndexes.push_back(lightsIndexes[i]);
		}
	}


	// Doodad references
	if (auto buffer = chunkReader->OpenChunk("MODR"))
	{
		_ASSERT(m_Header.flags.HAS_DOODADS);

		uint32 doodadsIndexesCount = buffer->getSize() / sizeof(uint16);
		uint16* doodadsIndexes = (uint16*)buffer->getDataFromCurrent();

		for (uint32 i = 0; i < doodadsIndexesCount; i++)
			m_DoodadsPlacementIndexes.push_back(doodadsIndexes[i]);
	}


	if (auto buffer = chunkReader->OpenChunk("MOBN"))
	{
		_ASSERT(m_Header.flags.HAS_COLLISION);

		collisionCount = buffer->getSize() / sizeof(SWMO_Group_MOBNDef);
		collisions = (SWMO_Group_MOBNDef*)buffer->getDataFromCurrent();

		for (uint32 i = 0; i < collisionCount; i++)
		{
			std::shared_ptr<CWMO_Group_Part_BSP_Node> collisionNode = std::make_shared<CWMO_Group_Part_BSP_Node>(*this, collisions[i]);
			m_CollisionNodes.push_back(collisionNode);
		}
	}


	if (auto buffer = chunkReader->OpenChunk("MOBR"))
	{
		uint32 indexesCnt = buffer->getSize() / sizeof(uint16);
		uint16* indices = (uint16*)buffer->getDataFromCurrent();

		/*collisionIndexes.reserve(indexesCnt * 3);
		for (uint32 i = 0; i < indexesCnt; i++)
		{
			collisionIndexes[i * 3 + 0] = dataFromMOVI[3 * indices[i] + 0];
			collisionIndexes[i * 3 + 1] = dataFromMOVI[3 * indices[i] + 1];
			collisionIndexes[i * 3 + 2] = dataFromMOVI[3 * indices[i] + 2];
		}*/
	}


	// Vertex colors
	if (auto buffer = chunkReader->OpenChunk("MOCV"))
	{
		_ASSERT(m_Header.flags.HAS_VERTEX_COLORS);
		uint32 vertexColorsCount = buffer->getSize() / sizeof(CBgra);
		CBgra* vertexColors = (CBgra*)buffer->getDataFromCurrent();

		mocv = new C4Vec[vertexColorsCount];
		memcpy(mocv, vertexColors, sizeof(C4Vec) * vertexColorsCount);
		mocv_count = vertexColorsCount;

		FixColorVertexAlpha(this);

		// Convert
		std::vector<vec4> vertexColorsConverted;
		for (uint32 i = 0; i < vertexColorsCount; i++)
		{
			vertexColorsConverted.push_back(vec4
			(
				static_cast<float>(mocv[i].z) / 255.0f,
				static_cast<float>(mocv[i].y) / 255.0f,
				static_cast<float>(mocv[i].x) / 255.0f,
				static_cast<float>(mocv[i].w) / 255.0f
			));
		}

		// Buffer
		VB_Colors = m_RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(vertexColorsConverted.data(), vertexColorsConverted.size(), 0, sizeof(vec4));
		m_IsMOCVExists = vertexColorsCount > 0;

		delete[] mocv;
	}


	if (auto buffer = chunkReader->OpenChunk("MLIQ"))
	{
		buffer->read(&m_LiquidHeader);

		Log::Green("WMO[%s]: LiquidType CHUNK = HEADER[%d] [%d]", m_WMOModel.getFilename().c_str(), m_Header.liquidType, m_WMOModel.m_Header.flags.use_liquid_type_dbc_id);

		uint32 liquid_type;
		if (m_WMOModel.m_Header.flags.use_liquid_type_dbc_id)
		{
			if (m_Header.liquidType < 21)
			{
				liquid_type = to_wmo_liquid(m_Header.liquidType - 1);
			}
			else
			{
				liquid_type = m_Header.liquidType;
			}
		}
		else
		{
			if (m_Header.liquidType < 20)
			{
				liquid_type = to_wmo_liquid(m_Header.liquidType);
			}
			else
			{
				liquid_type = m_Header.liquidType + 1;
			}
		}

		m_WMOLiqiud = std::make_shared<CWMO_Liquid>(m_RenderDevice, m_LiquidHeader.A, m_LiquidHeader.B);
		m_WMOLiqiud->CreateFromWMO(buffer, m_WMOModel.m_Materials[m_LiquidHeader.materialID], m_BaseManager->GetManager<CDBCStorage>()->DBC_LiquidType()[1], m_Header.flags.IS_INDOOR);

	}



	// Create geom
	{
		std::shared_ptr<IGeometry> geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
		geometry->AddVertexBuffer(BufferBinding("POSITION", 0), VB_Vertexes);
		geometry->AddVertexBuffer(BufferBinding("NORMAL", 0), VB_Normals);
		//if (VB_Colors != nullptr)
		geometry->AddVertexBuffer(BufferBinding("COLOR", 0), VB_Colors);
		geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), VB_TextureCoords[0]);
		//if (VB_TextureCoords.size() > 1)
		geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 1), VB_TextureCoords[1]);
		geometry->SetIndexBuffer(IB_Default);

		for (const auto& WMOGroupBatchProto : m_WMOBatchs)
		{
			std::shared_ptr<WMO_Group_Part_Batch> batch = std::make_shared<WMO_Group_Part_Batch>(m_RenderDevice, m_WMOModel, WMOGroupBatchProto);

			batch->AddConnection(m_WMOModel.m_Materials[WMOGroupBatchProto.material_id], geometry, SGeometryDrawArgs(WMOGroupBatchProto.indexStart, WMOGroupBatchProto.indexCount));

			m_WMOBatchIndexes.push_back(batch);
		}
		// TODO: enable me std::sort(m_WMOBatchIndexes.begin(), m_WMOBatchIndexes.end(), WMO_Group_Part_BatchCompare());
		m_WMOBatchs.clear();
	}
}
