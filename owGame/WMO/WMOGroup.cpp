#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include
#include "WMO.h"
#include "WMO_Group_Instance.h"

// General
#include "WMOGroup.h"

// Additional 
#include "WoWChunkReader.h"
#include "WMO_Base_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

#include "WMO_Fixes.h"

CWMOGroup::CWMOGroup(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CWMO& WMO, const uint32 GroupIndex, const SWMO_MOGI& GroupProto)
	: m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_WMO(WMO)
	, m_GroupIndex(GroupIndex)
{
	if (GroupProto.nameoffset != -1)
		m_GroupName = std::string(m_WMO.GetGroupName(GroupProto.nameoffset));
	else
		m_GroupName = m_WMO.GetFilename() + "_Group" + std::to_string(GroupIndex);

	m_Bounds = GroupProto.bounding_box.Convert();

	char temp[MAX_PATH];
	strcpy_s(temp, m_WMO.GetFilename().c_str());
	temp[m_WMO.GetFilename().length() - 4] = 0;

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
		buffer->readBytes(&m_GroupHeader, sizeof(SWMOGroup_MOGP));

		// Real wmo group file contains only 2 chunks: MVER and MOGP.
		// Start of MOGP is header (without fourcc).
		// After header data places others chunks.
		// We reinitialize chunk reader from current position
		// chunkReader.reset() DON'T call this, because source buffer will be free.
		m_ChunkReader = std::make_unique<WoWChunkReader>(m_BaseManager, buffer->getDataFromCurrent(), buffer->getSize() - sizeof(SWMOGroup_MOGP));
	}
}

CWMOGroup::~CWMOGroup()
{}



//
// CWMOGroup
//
const SWMOGroup_MOGP & CWMOGroup::GetHeader() const
{
	return m_GroupHeader;
}

const uint32 CWMOGroup::GetGroupIndex() const
{
	return m_GroupIndex;
}

const BoundingBox & CWMOGroup::GetBoundingBox() const
{
	return m_Bounds;
}

bool CWMOGroup::IsIndoor() const
{
	return m_GroupHeader.flags.IS_INDOOR;
}

bool CWMOGroup::IsOutdoor() const
{
	return m_GroupHeader.flags.IS_OUTDOOR;
}

void CWMOGroup::AddPortal(const CWMO_Part_Portal& WMOPartPortal)
{
	m_Portals.push_back(WMOPartPortal);
}

const std::vector<CWMO_Part_Portal>& CWMOGroup::GetPortals() const
{
	return m_Portals;
}



//
// ISceneNodeProvider
//
void CWMOGroup::CreateInsances(const std::shared_ptr<CWMO_Group_Instance>& Parent) const
{
	_ASSERT(GetState() == ILoadable::ELoadableState::Loaded);

	for (const auto& batch : m_WMOBatchIndexes)
	{
		Parent->GetComponentT<IModelComponent>()->AddModel(batch);
	}

	// WMO Group liquid
	if (m_WMOLiqiud != nullptr)
	{
		auto liquidInstance = Parent->CreateSceneNode<CWMO_Liquid_Instance>();

		m_WMOLiqiud->CreateInsances(liquidInstance);

		// Transform
		glm::vec3 realPos = Fix_XZmY(m_WMOLiqiud->GetHeader().pos);
		liquidInstance->SetPosition(glm::vec3(realPos.x, 0, realPos.z));

		// Collider
		if (auto liquidColliderComponent = liquidInstance->GetComponentT<IColliderComponent>())
		{
			BoundingBox bbox(
				glm::vec3(0.0f, realPos.y - 1.0f, -1.0f * m_WMOLiqiud->GetHeader().Y * C_UnitSize),
				glm::vec3(m_WMOLiqiud->GetHeader().X * C_UnitSize, realPos.y + 1.0f, 0.0f)
			);
			liquidColliderComponent->SetBounds(bbox);
		}

		Parent->AddRoomObject(liquidInstance);
	}

#if 1 && defined(USE_M2_MODELS)
	//std::vector<SWMO_MODS> activeDoodadSets;
	//activeDoodadSets.push_back(m_WMO.m_DoodadsSetInfos[0]);

	//if (parentAsWMOInstance->GetDoodadSetIndex() != -1)
	//	activeDoodadSets.push_back(m_WMO.m_DoodadsSetInfos[parentAsWMOInstance->GetDoodadSetIndex()]);

	// WMO Group M2s
	//for (const auto& doodadSet : activeDoodadSets)
	//{
		//for (size_t i = doodadSet.start; i < doodadSet.start + doodadSet.size; i++)
	for (size_t i = 0; i < m_DoodadsPlacementIndexes.size(); i++)
	{
		auto doodadPlacementIndex = m_DoodadsPlacementIndexes[i];

		const SWMO_MODD& placement = m_WMO.GetDoodadPlacement(doodadPlacementIndex);

		std::string doodadFileName = m_WMO.GetDoodadFileName(placement.flags.nameIndex);
		if (std::shared_ptr<CM2> m2 = m_BaseManager.GetManager<IWoWObjectsCreator>()->LoadM2(m_RenderDevice, doodadFileName, true))
		{
			auto inst = Parent->CreateSceneNode<CWMO_Doodad_Instance>(m2, doodadPlacementIndex, placement);

			if (false == m_GroupHeader.flags.DO_NOT_USE_LIGHTING_DIFFUSE && !m_GroupHeader.flags.IS_OUTDOOR)
				inst->setColor(placement.getColor());

			m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(inst);
			Parent->AddRoomObject(inst);
		}
	}
	//}
#endif
}

uint32 to_wmo_liquid(const SWMOGroup_MOGP& Header, int x)
{
	DBC_LIQUIDTYPE_Type basic = (DBC_LIQUIDTYPE_Type)(x & 3);
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

	throw CException("Unexpected behaviour");
}

const DBC_LiquidTypeRecord* ResolveLiquidType(const CDBCStorage* DBCStorage, const SWMO_MOHD& WMOHeader, const SWMOGroup_MOGP& WMOGroupHeader)
{
	uint32 liquid_type = UINT32_MAX;

	if (WMOHeader.flags.use_liquid_type_dbc_id != 0)
	{
		if (WMOGroupHeader.liquidType < 21)
		{
			liquid_type = to_wmo_liquid(WMOGroupHeader, WMOGroupHeader.liquidType - 1);
		}
		else
		{
			liquid_type = WMOGroupHeader.liquidType;
		}
	}
	else
	{
		if (WMOGroupHeader.liquidType == 15 /*Liquid green lava*/)
		{
			//return nullptr;
			liquid_type = 1;  // use to_wmo_liquid(SMOLTile->liquid) ? It seems to work alright.
		}
		else
		{
			if (WMOGroupHeader.liquidType < 20)
			{
				liquid_type = to_wmo_liquid(WMOGroupHeader, WMOGroupHeader.liquidType);
			}
			else
			{
				liquid_type = WMOGroupHeader.liquidType + 1;
			}
		}
	}

	return DBCStorage->DBC_LiquidType()[liquid_type];
}




bool CWMOGroup::Load()
{
	// Buffer
	uint16* dataFromMOVI = nullptr;
	glm::vec3 * dataFromMOVT = nullptr;

	// CollisionTEMP
	uint32 collisionCount = 0;
	SWMOGroup_MOBN* collisions = nullptr;

	std::shared_ptr<IGeometry> geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();

	// Material info for triangles
	for (const auto& materialsInfo : m_ChunkReader->OpenChunkT<SWMOGroup_MOPY>("MOPY")) // Material info for triangles
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
	for (const auto& buffer : m_ChunkReader->OpenChunks("MOTV"))
	{
		geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer((const glm::vec2*)buffer->getData(), buffer->getSize() / sizeof(glm::vec2)));
		break;
	}


	// WMO_Group_Batch
	std::vector<SWMOGroup_MOBA> WMOBatchs;
	for (const auto& batchDef : m_ChunkReader->OpenChunkT<SWMOGroup_MOBA>("MOBA"))
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
		m_DoodadsPlacementIndexes.assign(doodadsIndexes, doodadsIndexes + doodadsIndexesCount);
	}


	for (const auto& collision : m_ChunkReader->OpenChunkT<SWMOGroup_MOBN>("MOBN"))
	{
		std::shared_ptr<CWMOGroup_Part_CollisionNode> collisionNode = MakeShared(CWMOGroup_Part_CollisionNode, *this, collision);
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
	auto mocvChunks = m_ChunkReader->OpenChunks("MOCV");
	for (size_t i = 0; i < mocvChunks.size(); i++)
	{
		_ASSERT(m_GroupHeader.flags.HAS_MOCV);

		const auto& currentMOCVChunk = mocvChunks.at(i);
		uint32 vertexColorsCount = currentMOCVChunk->getSize() / sizeof(CBgra);
		CBgra* vertexColors = (CBgra*)currentMOCVChunk->getDataFromCurrent();

#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
		if (false == m_WMO.GetHeader().flags.lighten_interiors)
			WMOGroupFixColors(m_GroupHeader, vertexColors, vertexColorsCount, WMOBatchs.data());
#endif

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

		geometry->AddVertexBuffer(BufferBinding("COLOR", i), m_RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(vertexColorsConverted.data(), vertexColorsConverted.size(), 0, sizeof(glm::vec4)));
		break;
	}
	mocvChunks.clear();


	if (auto buffer = m_ChunkReader->OpenChunk("MLIQ"))
	{
		SWMOGroup_MLIQ liquidHeader;
		buffer->read(&liquidHeader);

		/*Log::Green
		(
			"WMO[%s]: Liq: headerID [%d] headerFlag [%d] MatID: [%d]",
			m_WMO.GetFilename().c_str(),
			m_GroupHeader.liquidType,
			m_WMO.GetHeader().flags.use_liquid_type_dbc_id,
			liquidHeader.materialID
		);*/

		m_WMOLiqiud = MakeShared(CWMOGroup_Part_Liquid, m_RenderDevice, m_WMO, *this, buffer, liquidHeader, ResolveLiquidType(m_BaseManager.GetManager<CDBCStorage>(), m_WMO.GetHeader(), m_GroupHeader));
	}

	/*Log::Info("WMO '%s': Batch count '%d'. Trans: '%d' (%d - %d). Int: '%d' (%d - %d). Ext: '%d' (%d - %d).",
		m_WMO.GetFilename().c_str(),
		WMOBatchs.size(),

		m_GroupHeader.transBatchCount,
		m_GroupHeader.transBatchCount > 0 ? WMOBatchs[0].vertexStart : -1,
		m_GroupHeader.transBatchCount > 0 ? WMOBatchs[0].vertexEnd : -1,

		m_GroupHeader.intBatchCount,
		m_GroupHeader.intBatchCount > 0 ? WMOBatchs[m_GroupHeader.transBatchCount].vertexStart : -1,
		m_GroupHeader.intBatchCount > 0 ? WMOBatchs[m_GroupHeader.transBatchCount].vertexEnd : -1,

		m_GroupHeader.extBatchCount,
		m_GroupHeader.extBatchCount > 0 ? WMOBatchs[m_GroupHeader.transBatchCount + m_GroupHeader.intBatchCount].vertexStart : -1,
		m_GroupHeader.extBatchCount > 0 ? WMOBatchs[m_GroupHeader.transBatchCount + m_GroupHeader.intBatchCount].vertexEnd : -1
	);
	Log::Info("WMO '%s': MOCV count '%d'.", m_WMO.GetFilename().c_str(), ccccccccc);*/

	if (m_GroupHeader.transBatchCount + m_GroupHeader.intBatchCount + m_GroupHeader.extBatchCount != WMOBatchs.size())
		throw CException("dick");

	size_t currentIntex = 0;
	for (const auto& WMOGroupBatchProto : WMOBatchs)
	{
		auto batch = MakeShared(WMO_Group_Part_Batch, m_RenderDevice, m_WMO, WMOGroupBatchProto);
		if (currentIntex < m_GroupHeader.transBatchCount)
		{
			batch->SetBatchType(WMO_Group_Part_Batch::EBatchType::BatchType_Trans);
		}
		else if (currentIntex < m_GroupHeader.transBatchCount + m_GroupHeader.intBatchCount)
		{
			batch->SetBatchType(WMO_Group_Part_Batch::EBatchType::BatchType_Int);
		}
		else if (currentIntex < m_GroupHeader.transBatchCount + m_GroupHeader.intBatchCount + m_GroupHeader.extBatchCount)
		{
			batch->SetBatchType(WMO_Group_Part_Batch::EBatchType::BatchType_Ext);
		}

		batch->AddConnection(m_WMO.GetMaterial(WMOGroupBatchProto.material_id), geometry, SGeometryDrawArgs(WMOGroupBatchProto.indexStart, WMOGroupBatchProto.indexCount));

		m_WMOBatchIndexes.push_back(batch);
		currentIntex++;
	}


	// TODO: enable me std::sort(m_WMOBatchIndexes.begin(), m_WMOBatchIndexes.end(), WMO_Group_Part_BatchCompare());
	WMOBatchs.clear();

	m_ChunkReader.reset();

	return true;
}

bool CWMOGroup::Delete()
{
	return false;
}

#endif
