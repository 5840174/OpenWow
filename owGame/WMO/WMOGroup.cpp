#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include
#include "WMO.h"
#include "WMO_Group_Instance.h"
#include "World/WorldClient.h"

// General
#include "WMOGroup.h"

// Additional 
#include "WMO_Base_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"

#include "WMOHelper.h"

CWMOGroup::CWMOGroup(const IBaseManager& BaseManager, IRenderDevice& RenderDevice, const CWMO& WMO, const uint32 GroupIndex, const SWMO_MOGI& GroupProto)
	: m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_WMO(WMO)
	, m_GroupIndex(GroupIndex)
{
	if (GroupProto.nameoffset != -1)
		m_GroupName = m_WMO.GetGroupName(GroupProto.nameoffset);
	else
		m_GroupName = m_WMO.GetFilename() + "_Group" + std::to_string(GroupIndex);

	m_Bounds = GroupProto.bounding_box.Convert();

	char wmoFilenameWithoutExtension[MAX_PATH];
	strcpy_s(wmoFilenameWithoutExtension, m_WMO.GetFilename().c_str());
	wmoFilenameWithoutExtension[m_WMO.GetFilename().length() - 4] = '\0';

	char groupFilename[MAX_PATH];
	sprintf_s(groupFilename, "%s_%03d.wmo", wmoFilenameWithoutExtension, m_GroupIndex);

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

const std::shared_ptr<IGeometry>& CWMOGroup::GetCollisionGeometry() const
{
	return m_CollisionGeom;
}

const std::vector<std::shared_ptr<CWMOGroup_Part_CollisionNode>>& CWMOGroup::GetCollisionNodes() const
{
	return m_CollisionNodes;
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
		glm::vec3 realPos = Fix_From_XZmY_To_XYZ(m_WMOLiqiud->GetHeader().pos);
		liquidInstance->SetPosition(glm::vec3(realPos.x, 0, realPos.z));

		// Collider
		BoundingBox bbox(
			glm::vec3(0.0f, realPos.y - 1.0f, -1.0f * m_WMOLiqiud->GetHeader().Y * C_UnitSize),
			glm::vec3(m_WMOLiqiud->GetHeader().X * C_UnitSize, realPos.y + 1.0f, 0.0f)
		);
		liquidInstance->SetBounds(bbox);

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
		if (auto m2 = Parent->GetWorldClient().GetCreator()->LoadM2(m_RenderDevice, doodadFileName))
		{
			auto inst = Parent->CreateSceneNode<CWMO_Doodad_Instance>(Parent->GetWorldClient(), m2, doodadPlacementIndex, placement);

			if (false == m_GroupHeader.flags.DO_NOT_USE_LIGHTING_DIFFUSE && !m_GroupHeader.flags.IS_OUTDOOR)
				inst->SetColor(placement.getColor());

			m_BaseManager.GetManager<ILoader>()->AddToLoadQueue(inst);
			Parent->AddRoomObject(inst);
		}
	}
	//}
#endif
}

bool CWMOGroup::Load()
{
	std::shared_ptr<IGeometry> geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();

	// Material info for triangles
	for (const auto& materialsInfo : m_ChunkReader->OpenChunkT<SWMOGroup_MOPY>("MOPY"))
	{
		m_MaterialsInfo.push_back(materialsInfo);
	}


	// Indices
	std::vector<uint16> indicesArray;
	if (auto buffer = m_ChunkReader->OpenChunk("MOVI"))
	{
		const uint32 indexesCount = buffer->getSize() / sizeof(uint16);
		const uint16* indexes = (const uint16*)buffer->getData();

		indicesArray.assign(indexes, indexes + indexesCount);

		geometry->SetIndexBuffer(m_RenderDevice.GetObjectsFactory().CreateIndexBuffer(indicesArray));
	}


	// Vertices
	std::shared_ptr<IBuffer> verticesBuffer = nullptr;
	std::vector<glm::vec3> verticesArray;
	if (auto buffer = m_ChunkReader->OpenChunk("MOVT"))
	{
		const uint32 vertexesCount = buffer->getSize() / sizeof(glm::vec3);
		const glm::vec3* vertexes = (const glm::vec3*)buffer->getData();
	
		verticesArray.resize(vertexesCount);
		for (uint32 i = 0; i < vertexesCount; i++)
			verticesArray[i] = Fix_From_XZmY_To_XYZ(vertexes[i]);

		// Buffer
		verticesBuffer = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(verticesArray);
		geometry->AddVertexBuffer(BufferBinding("POSITION", 0), verticesBuffer);
	}


	// Normals
	if (auto buffer = m_ChunkReader->OpenChunk("MONR"))
	{
		const uint32 normalsCount = buffer->getSize() / sizeof(glm::vec3);
		const glm::vec3* normals = (const glm::vec3*)buffer->getDataFromCurrent();

		std::vector<glm::vec3> normalsArray;
		normalsArray.resize(normalsCount);
		for (uint32 i = 0; i < normalsCount; i++)
			normalsArray[i] = Fix_From_XZmY_To_XYZ(normals[i]);

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

	// Collision indices
	std::vector<uint16> collisionIndicesArray;
	if (auto buffer = m_ChunkReader->OpenChunk("MOBR"))
	{
		uint32 collisionIndicesCnt = buffer->getSize() / sizeof(uint16);
		uint16* collisionIndices = (uint16*)buffer->getDataFromCurrent();

		collisionIndicesArray.resize(collisionIndicesCnt * 3);
		for (uint32 i = 0; i < collisionIndicesCnt; i++)
			for (uint8 j = 0; j < 3; j++)
				collisionIndicesArray[(i * 3) + j] = indicesArray[(3 * collisionIndices[i]) + j];

		// Collision index buffer
		auto collisitonIndexBuffer = m_RenderDevice.GetObjectsFactory().CreateIndexBuffer(collisionIndicesArray);

		m_CollisionGeom = m_RenderDevice.GetObjectsFactory().CreateGeometry();
		m_CollisionGeom->SetVertexBuffer(verticesBuffer);
		m_CollisionGeom->SetIndexBuffer(collisitonIndexBuffer);
	}

	// CollisionNodes
	for (const auto& collision : m_ChunkReader->OpenChunkT<SWMOGroup_MOBN>("MOBN"))
	{
		auto collisionNode = MakeShared(CWMOGroup_Part_CollisionNode, m_RenderDevice, *this, collision, verticesArray, collisionIndicesArray);
		m_CollisionNodes.push_back(collisionNode);
	}


	// Vertex colors
	auto mocvChunks = m_ChunkReader->OpenChunks("MOCV");
	//if (mocvChunks.size() > 1)
	//	throw CException("MOCV count more then 1!");

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

		m_WMOLiqiud = MakeShared(CWMOGroup_Part_Liquid, m_RenderDevice, m_WMO, *this, buffer, liquidHeader, WMOGroupResolveLiquidType(m_BaseManager.GetManager<CDBCStorage>(), m_WMO.GetHeader(), m_GroupHeader));
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
