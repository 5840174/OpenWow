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

WMO_Group::WMO_Group(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<CWMO>& WMOModel, const uint32 GroupIndex, const SWMO_GroupInfoDef& GroupProto)
	: CLoadableObject(WMOModel)
	, m_IsMOCVExists(false)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_WMOModel(*WMOModel)
	, m_GroupIndex(GroupIndex)
{
	if (GroupProto.nameoffset != -1)
		m_GroupName = std::string(m_WMOModel.m_GroupNames.get() + GroupProto.nameoffset);
	else
		m_GroupName = m_WMOModel.getFilename() + "_Group" + std::to_string(GroupIndex);

	m_Bounds = GroupProto.bounding_box.Convert();

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
		buffer->readBytes(&m_GroupHeader, sizeof(SWMO_Group_HeaderDef));
		_ASSERT(m_GroupHeader.flags.HAS_3_MOTV == 0);

		// Real wmo group file contains only 2 chunks: MVER and MOGP.
		// Start of MOGP is header (without fourcc).
		// After header data places others chunks.
		// We reinitialize chunk reader from current position
		// chunkReader.reset() DON'T call this, because source buffer will be free.
		m_ChunkReader = std::make_unique<WoWChunkReader>(m_BaseManager, buffer->getDataFromCurrent(), buffer->getSize() - sizeof(SWMO_Group_HeaderDef));
	}
}

WMO_Group::~WMO_Group()
{
}



//
// WMO_Group
//
const uint32 WMO_Group::GetGroupIndex() const
{
	return m_GroupIndex;
}

void WMO_Group::AddPortal(const CWMO_Part_Portal& WMOPartPortal)
{
	m_Portals.push_back(WMOPartPortal);
}

const std::vector<CWMO_Part_Portal>& WMO_Group::GetPortals() const
{
	return m_Portals;
}



//
// ISceneNodeProvider
//
void WMO_Group::CreateInsances(const std::shared_ptr<ISceneNode3D>& Parent) const
{
	_ASSERT(GetState() == ILoadable::ELoadableState::Loaded);

	auto parentAsWMOGroupInstance = std::dynamic_pointer_cast<CWMO_Group_Instance>(Parent);
	_ASSERT(parentAsWMOGroupInstance != nullptr);

	for (const auto& batch : m_WMOBatchIndexes)
	{
		parentAsWMOGroupInstance->GetComponent<IModelsComponent3D>()->AddModel(batch);
	}

	// WMO Group liquid
	if (m_WMOLiqiud != nullptr)
	{
		glm::vec3 realPos = Fix_XZmY(m_WMOLiqiud->GetHeader().pos);

		auto liquidInstance = Parent->CreateSceneNode<CWMO_Liquid_Instance>();
		m_WMOLiqiud->CreateInsances(liquidInstance);

		// Transform
		liquidInstance->SetTranslate(glm::vec3(realPos.x, 0, realPos.z));

		// IColliderComponent3D
		{
			BoundingBox bbox = parentAsWMOGroupInstance->GetColliderComponent()->GetBounds();
			bbox.setMin(bbox.getMin() - realPos);
			bbox.setMax(bbox.getMax() - realPos);
			bbox.calculateCenter();
			liquidInstance->GetColliderComponent()->SetBounds(bbox);
		}

		parentAsWMOGroupInstance->AddRoomObject(liquidInstance);
		
	}

#if 1
#ifdef USE_M2_MODELS
	// WMO Group M2s
	for (const auto& index : m_DoodadsPlacementIndexes)
	{
		const SWMO_Doodad_PlacementInfo& placement = m_WMOModel.m_DoodadsPlacementInfos[index];

		std::string doodadFileName = m_WMOModel.m_DoodadsFilenames + placement.flags.nameIndex;

		std::shared_ptr<CM2> m2 = m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, doodadFileName, true);
		if (m2)
		{
			auto inst = Parent->CreateSceneNode<CWMO_Doodad_Instance>(m2, index, placement);
			inst->Load();
			inst->SetState(ELoadableState::Loaded);
			//m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(inst);
			parentAsWMOGroupInstance->AddRoomObject(inst);
		}
	}
#endif
#endif
}

uint32 to_wmo_liquid(const SWMO_Group_HeaderDef& Header, int x)
{
	DBC_LIQUIDTYPE_Type::List basic = (DBC_LIQUIDTYPE_Type::List)(x & 3);
	switch (basic)
	{
	case DBC_LIQUIDTYPE_Type::water:
		return (Header.flags.IS_NOT_WATER_BUT_OCEAN) ? 14 : 13;
	case DBC_LIQUIDTYPE_Type::ocean:
		return 14;
	case DBC_LIQUIDTYPE_Type::magma:
		return 19;
	case DBC_LIQUIDTYPE_Type::slime:
		return 20;
	}

	return UINT32_MAX;
}

bool WMO_Group::Load()
{
	// Buffer
	uint16* dataFromMOVI = nullptr;
	dataFromMOVT = nullptr;

	// CollisionTEMP
	uint32 collisionCount = 0;
	SWMO_Group_MOBNDef* collisions = nullptr;

	std::shared_ptr<IGeometry> geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();

	// Material info for triangles
	for (const auto& materialsInfo : m_ChunkReader->OpenChunkT<SWMO_Group_MaterialDef>("MOPY")) // Material info for triangles
	{
		m_MaterialsInfo.push_back(materialsInfo);
	}


	// Indices
	if (auto buffer = m_ChunkReader->OpenChunk("MOVI"))
	{
		// Buffer
		geometry->SetIndexBuffer(m_RenderDevice.GetObjectsFactory().CreateIndexBuffer((const uint16*)buffer->getData(), buffer->getSize() / sizeof(uint16)));
	}


	// Vertices chunk.
	if (auto buffer = m_ChunkReader->OpenChunk("MOVT"))
	{
		uint32 vertexesCount = buffer->getSize() / sizeof(glm::vec3);
		glm::vec3* vertexes = (glm::vec3*)buffer->getData();

		// Convert
		for (uint32 i = 0; i < vertexesCount; i++)
			vertexes[i] = Fix_XZmY(vertexes[i]);

		// Buffer
		geometry->AddVertexBuffer(BufferBinding("POSITION", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(vertexes, vertexesCount));

		std::vector<glm::vec4> colors;
		colors.resize(vertexesCount);
		std::fill(colors.begin(), colors.end(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		geometry->AddVertexBuffer(BufferBinding("COLOR", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(colors));

		dataFromMOVT = vertexes;
	}


	// Normals
	if (auto buffer = m_ChunkReader->OpenChunk("MONR"))
	{
		uint32 normalsCount = buffer->getSize() / sizeof(glm::vec3);
		glm::vec3* normals = (glm::vec3*)buffer->getDataFromCurrent();

		// Convert
		for (uint32 i = 0; i < normalsCount; i++)
			normals[i] = Fix_XZmY(normals[i]);

		// Buffer
		geometry->AddVertexBuffer(BufferBinding("NORMAL", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(normals, normalsCount));
	}


	// Texture coords
	if (auto buffer = m_ChunkReader->OpenChunk("MOTV"))
	{
		geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer((const glm::vec2*)buffer->getData(), buffer->getSize() / sizeof(glm::vec2)));
	}


	// WMO_Group_Batch
	std::vector<SWMO_Group_BatchDef> WMOBatchs;
	for (const auto& batchDef : m_ChunkReader->OpenChunkT<SWMO_Group_BatchDef>("MOBA"))
	{
		WMOBatchs.push_back(batchDef);
	}


	// Light references
	if (auto buffer = m_ChunkReader->OpenChunk("MOLR"))
	{
		_ASSERT(m_GroupHeader.flags.HAS_LIGHTS);
		uint32 lightsIndexesCount = buffer->getSize() / sizeof(uint16);
		uint16* lightsIndexes = (uint16*)buffer->getDataFromCurrent();
		for (uint32 i = 0; i < lightsIndexesCount; i++)
		{
			m_WMOLightsIndexes.push_back(lightsIndexes[i]);
		}
	}


	// Doodad references
	if (auto buffer = m_ChunkReader->OpenChunk("MODR"))
	{
		_ASSERT(m_GroupHeader.flags.HAS_DOODADS);

		uint32 doodadsIndexesCount = buffer->getSize() / sizeof(uint16);
		uint16* doodadsIndexes = (uint16*)buffer->getDataFromCurrent();

		for (uint32 i = 0; i < doodadsIndexesCount; i++)
			m_DoodadsPlacementIndexes.push_back(doodadsIndexes[i]);
	}


	for (const auto& collision : m_ChunkReader->OpenChunkT<SWMO_Group_MOBNDef>("MOBN"))
	{
		std::shared_ptr<CWMO_Group_Part_BSP_Node> collisionNode = std::make_shared<CWMO_Group_Part_BSP_Node>(*this, collision);
		m_CollisionNodes.push_back(collisionNode);
	}


	if (auto buffer = m_ChunkReader->OpenChunk("MOBR"))
	{
		uint32 indexesCnt = buffer->getSize() / sizeof(uint16);
		uint16* indices = (uint16*)buffer->getDataFromCurrent();

		// Buffer
		//geometry->SetIndexBuffer(m_RenderDevice.GetObjectsFactory().CreateIndexBuffer((const uint16*)buffer->getData(), buffer->getSize() / sizeof(uint16)));

		/*collisionIndexes.reserve(indexesCnt * 3);
		for (uint32 i = 0; i < indexesCnt; i++)
		{
			collisionIndexes[i * 3 + 0] = dataFromMOVI[3 * indices[i] + 0];
			collisionIndexes[i * 3 + 1] = dataFromMOVI[3 * indices[i] + 1];
			collisionIndexes[i * 3 + 2] = dataFromMOVI[3 * indices[i] + 2];
		}*/
	}


	// Vertex colors
	if (auto buffer = m_ChunkReader->OpenChunk("MOCV"))
	{
		_ASSERT(m_GroupHeader.flags.HAS_VERTEX_COLORS);

		uint32 vertexColorsCount = buffer->getSize() / sizeof(CBgra);
		CBgra* vertexColors = (CBgra*)buffer->getDataFromCurrent();

		// Convert
		std::vector<glm::vec4> vertexColorsConverted;
		for (uint32 i = 0; i < vertexColorsCount; i++)
		{
			vertexColorsConverted.push_back(glm::vec4(
				static_cast<float>(vertexColors[i].r) / 255.0f,
				static_cast<float>(vertexColors[i].g) / 255.0f,
				static_cast<float>(vertexColors[i].b) / 255.0f,
				static_cast<float>(vertexColors[i].a) / 255.0f
			));
		}

		geometry->AddVertexBuffer(BufferBinding("COLOR", 0), m_RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(vertexColorsConverted.data(), vertexColorsConverted.size(), 0, sizeof(glm::vec4)));
		m_IsMOCVExists = vertexColorsCount > 0;
	}


	if (auto buffer = m_ChunkReader->OpenChunk("MLIQ"))
	{
		SWMO_Group_MLIQDef liquidHeader;
		buffer->read(&liquidHeader);

		Log::Green
		(
			"WMO[%s]: Liq: headerID [%d] headerFlag [%d] MatID: [%d] MatShader[%d]", 
			m_WMOModel.getFilename().c_str(),
			m_GroupHeader.liquidType,
			m_WMOModel.m_Header.flags.use_liquid_type_dbc_id, 
			liquidHeader.materialID,
			m_WMOModel.m_Materials[liquidHeader.materialID]->GetProto().shader
		);

		uint32 liquid_type;
		if (m_WMOModel.m_Header.flags.use_liquid_type_dbc_id != 0)
		{
			if (m_GroupHeader.liquidType < 21)
			{
				liquid_type = to_wmo_liquid(m_GroupHeader, m_GroupHeader.liquidType - 1);
			}
			else
			{
				liquid_type = m_GroupHeader.liquidType;
			}
		}
		else
		{
			if (m_GroupHeader.liquidType < 20)
			{
				liquid_type = to_wmo_liquid(m_GroupHeader, m_GroupHeader.liquidType);
			}
			else
			{
				liquid_type = m_GroupHeader.liquidType + 1;
			}
		}

		m_WMOLiqiud = std::make_shared<CWMO_Liquid>(m_RenderDevice, m_WMOModel, *this, buffer, liquidHeader);
	}

	for (const auto& WMOGroupBatchProto : WMOBatchs)
	{
		std::shared_ptr<WMO_Group_Part_Batch> batch = std::make_shared<WMO_Group_Part_Batch>(m_RenderDevice, m_WMOModel, WMOGroupBatchProto);

		batch->AddConnection(m_WMOModel.m_Materials[WMOGroupBatchProto.material_id], geometry, SGeometryDrawArgs(WMOGroupBatchProto.indexStart, WMOGroupBatchProto.indexCount));

		m_WMOBatchIndexes.push_back(batch);
	}
	// TODO: enable me std::sort(m_WMOBatchIndexes.begin(), m_WMOBatchIndexes.end(), WMO_Group_Part_BatchCompare());
	WMOBatchs.clear();


	m_ChunkReader.reset();

	return true;
}
