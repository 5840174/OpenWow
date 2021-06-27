#include "stdafx.h"

#ifdef USE_WMO_MODELS

// Include
#include "WMO.h"
#include "WMO_Group_Instance.h"

// General
#include "WMO_Group.h"

// Additional 
#include "WoWChunkReader.h"
#include "WMO_Base_Instance.h"
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"
#include "WMO_Fixes.h"

CWMO_Group::CWMO_Group(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::shared_ptr<CWMO>& WMOModel, const uint32 GroupIndex, const SWMO_GroupInfoDef& GroupProto)
	: CLoadableObject(WMOModel)
	, m_IsMOCVExists(false)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_WMOModel(*WMOModel)
	, m_GroupIndex(GroupIndex)
{
	if (GroupProto.nameoffset != -1)
		m_GroupName = std::string(m_WMOModel.GetGroupName(GroupProto.nameoffset));
	else
		m_GroupName = m_WMOModel.GetFilename() + "_Group" + std::to_string(GroupIndex);

	m_Bounds = GroupProto.bounding_box.Convert();

	char temp[MAX_PATH];
	strcpy_s(temp, m_WMOModel.GetFilename().c_str());
	temp[m_WMOModel.GetFilename().length() - 4] = 0;

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

CWMO_Group::~CWMO_Group()
{}



//
// CWMO_Group
//
const SWMO_Group_HeaderDef & CWMO_Group::GetHeader() const
{
	return m_GroupHeader;
}

const uint32 CWMO_Group::GetGroupIndex() const
{
	return m_GroupIndex;
}

const BoundingBox & CWMO_Group::GetBoundingBox() const
{
	return m_Bounds;
}

bool CWMO_Group::IsIndoor() const
{
	return m_GroupHeader.flags.IS_INDOOR;
}

bool CWMO_Group::IsOutdoor() const
{
	return m_GroupHeader.flags.IS_OUTDOOR;
}

void CWMO_Group::AddPortal(const CWMO_Part_Portal& WMOPartPortal)
{
	m_Portals.push_back(WMOPartPortal);
}

const std::vector<CWMO_Part_Portal>& CWMO_Group::GetPortals() const
{
	return m_Portals;
}



//
// ISceneNodeProvider
//
void CWMO_Group::CreateInsances(const std::shared_ptr<CWMO_Group_Instance>& Parent) const
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
	//std::vector<SWMO_Doodad_SetInfo> activeDoodadSets;
	//activeDoodadSets.push_back(m_WMOModel.m_DoodadsSetInfos[0]);

	//if (parentAsWMOInstance->GetDoodadSetIndex() != -1)
	//	activeDoodadSets.push_back(m_WMOModel.m_DoodadsSetInfos[parentAsWMOInstance->GetDoodadSetIndex()]);

	// WMO Group M2s
	//for (const auto& doodadSet : activeDoodadSets)
	//{
		//for (size_t i = doodadSet.start; i < doodadSet.start + doodadSet.size; i++)
	for (size_t i = 0; i < m_DoodadsPlacementIndexes.size(); i++)
	{
		auto doodadPlacementIndex = m_DoodadsPlacementIndexes[i];

		const SWMO_Doodad_PlacementInfo& placement = m_WMOModel.GetDoodadPlacement(doodadPlacementIndex);

		std::string doodadFileName = m_WMOModel.GetDoodadFileName(placement.flags.nameIndex);
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

uint32 to_wmo_liquid(const SWMO_Group_HeaderDef& Header, int x)
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

const DBC_LiquidTypeRecord* ResolveLiquidType(const CDBCStorage* DBCStorage, const SWMO_HeaderDef& WMOHeader, const SWMO_Group_HeaderDef& WMOGroupHeader)
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




bool CWMO_Group::Load()
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
	for (const auto& buffer : m_ChunkReader->OpenChunks("MOTV"))
	{
		geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer((const glm::vec2*)buffer->getData(), buffer->getSize() / sizeof(glm::vec2)));
		break;
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
		m_DoodadsPlacementIndexes.assign(doodadsIndexes, doodadsIndexes + doodadsIndexesCount);
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
	uint32 ccccccccc = 0;
	for (const auto& buffer : m_ChunkReader->OpenChunks("MOCV"))
	{
		_ASSERT(m_GroupHeader.flags.HAS_MOCV);

		uint32 vertexColorsCount = buffer->getSize() / sizeof(CBgra);
		CBgra* vertexColors = (CBgra*)buffer->getDataFromCurrent();

		ccccccccc = vertexColorsCount;

#if WOW_CLIENT_VERSION >= WOW_WOTLK_3_3_5
		if (false == m_WMOModel.GetHeader().flags.lighten_interiors)
			FixColors(vertexColors, vertexColorsCount, WMOBatchs.data());
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

		geometry->AddVertexBuffer(BufferBinding("COLOR", 0), m_RenderDevice.GetObjectsFactory().CreateVoidVertexBuffer(vertexColorsConverted.data(), vertexColorsConverted.size(), 0, sizeof(glm::vec4)));
		m_IsMOCVExists = vertexColorsCount > 0;
		break;
	}


	if (auto buffer = m_ChunkReader->OpenChunk("MLIQ"))
	{
		SWMO_Group_MLIQDef liquidHeader;
		buffer->read(&liquidHeader);

		/*Log::Green
		(
			"WMO[%s]: Liq: headerID [%d] headerFlag [%d] MatID: [%d]",
			m_WMOModel.GetFilename().c_str(),
			m_GroupHeader.liquidType,
			m_WMOModel.GetHeader().flags.use_liquid_type_dbc_id,
			liquidHeader.materialID
		);*/



		m_WMOLiqiud = std::make_shared<CWMO_Liquid>(m_RenderDevice, m_WMOModel, *this, buffer, liquidHeader, ResolveLiquidType(m_BaseManager.GetManager<CDBCStorage>(), m_WMOModel.GetHeader(), m_GroupHeader));
	}

	/*Log::Info("WMO '%s': Batch count '%d'. Trans: '%d' (%d - %d). Int: '%d' (%d - %d). Ext: '%d' (%d - %d).",
		m_WMOModel.GetFilename().c_str(),
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
	Log::Info("WMO '%s': MOCV count '%d'.", m_WMOModel.GetFilename().c_str(), ccccccccc);*/




	if (m_GroupHeader.transBatchCount + m_GroupHeader.intBatchCount + m_GroupHeader.extBatchCount != WMOBatchs.size())
		throw CException("dick");

	size_t currentIntex = 0;
	for (const auto& WMOGroupBatchProto : WMOBatchs)
	{
		std::shared_ptr<WMO_Group_Part_Batch> batch = std::make_shared<WMO_Group_Part_Batch>(m_RenderDevice, m_WMOModel, WMOGroupBatchProto);
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

		batch->AddConnection(m_WMOModel.GetMaterial(WMOGroupBatchProto.material_id), geometry, SGeometryDrawArgs(WMOGroupBatchProto.indexStart, WMOGroupBatchProto.indexCount));

		m_WMOBatchIndexes.push_back(batch);
		currentIntex++;
	}


	// TODO: enable me std::sort(m_WMOBatchIndexes.begin(), m_WMOBatchIndexes.end(), WMO_Group_Part_BatchCompare());
	WMOBatchs.clear();

	m_ChunkReader.reset();

	return true;
}


#define epsilon 0.01F

enum Flags
{
	Flag_XAxis = 0x0,
	Flag_YAxis = 0x1,
	Flag_ZAxis = 0x2,
	Flag_AxisMask = 0x3,
	Flag_Leaf = 0x4,
	Flag_NoChild = 0xFFFF,
};

struct CAaBspNode
{
	uint16_t flags;        // See above enum. 4: leaf, 0 for YZ-plane, 1 for XZ-plane, 2 for XY-plane
	int16_t negChild;      // index of bsp child node (right in this array)
	int16_t posChild;
	uint16_t nFaces;       // num of triangle faces in MOBR
	uint32_t faceStart;    // index of the first triangle index(in MOBR)
	float planeDist;
};

void MergeBox(glm::vec3(&result)[2], float  *box1, float  *box2)
{
	result[0][0] = box1[0];
	result[0][1] = box1[1];
	result[0][2] = box1[2];

	result[1][0] = box2[0];
	result[1][1] = box2[1];
	result[1][2] = box2[2];
}
void AjustDelta(glm::vec3(&src)[2], float *dst, float coef)
{
	float d1 = (src[1][0] - src[0][0]) * coef;// delta x
	float d2 = (src[1][1] - src[0][1]) * coef;// delta y
	float d3 = (src[1][2] - src[0][2]) * coef;// delta z
	dst[1] = d1 + src[0][1];
	dst[0] = d2 + src[0][0];
	dst[2] = d3 + src[0][2];
}
void TraverseBsp(int iNode, glm::vec3(&pEyes)[2], glm::vec3(&pBox)[2], void *(pAction)(CAaBspNode*, void*), void* param)
{
	glm::vec3 newEyes[2];
	glm::vec3 ajusted;
	CAaBspNode *pNode = nullptr; // &m_tNode[iNode];

	if (pNode == nullptr)
		return;

	if (pNode->flags & Flag_Leaf)
	{
		if (pAction == nullptr)
		{
			//RenderGeometry(GetEngine3DInstance(), pNode);
			return;
		}
		else
		{
			pAction(pNode, param);
		}
	}

	int plane = pNode->flags & Flag_AxisMask;

	float eyesmin_boxmin = pEyes[0][plane] - pBox [0][plane];
	float boxmax_eyesmax =  pBox[1][plane] - pEyes[1][plane];

	if (false == ((-epsilon < eyesmin_boxmin) | (-epsilon == eyesmin_boxmin)) && false == ((pEyes[1][plane] - pBox [0][plane]) >= -epsilon) && 
		false == (( epsilon < boxmax_eyesmax) | ( epsilon == boxmax_eyesmax)) && false == (( pBox[1][plane] - pEyes[0][plane]) >= epsilon)
		)
		return;

	glm::vec3 tBox1[2];
	memmove(tBox1, pBox, sizeof(pBox));
	tBox1[0][plane] = pNode->planeDist;

	glm::vec3 tBox2[2];
	memmove(tBox2, pBox, sizeof(pBox));
	tBox2[1][plane] = pNode->planeDist;

	float eyes_min_fdist = (pEyes[0][plane]) - pNode->planeDist;
	float eyes_max_fdist = (pEyes[1][plane]) - pNode->planeDist;

	if (((eyes_min_fdist >= -epsilon) && (eyes_min_fdist <= epsilon)) || ((eyes_max_fdist >= -epsilon) && (eyes_max_fdist <= epsilon)))
	{
		if (pNode->posChild != -1) 
			TraverseBsp(pNode->posChild, pEyes, tBox1, pAction, param);
		if (pNode->negChild != -1) 
			TraverseBsp(pNode->negChild, pEyes, tBox2, pAction, param);
		return;
	}

	if (eyes_min_fdist > epsilon && eyes_max_fdist < epsilon)
	{
		if (pNode->posChild != -1) 
			TraverseBsp(pNode->posChild, pEyes, tBox1, pAction, param);
		return;
	}

	if (eyes_min_fdist < -epsilon && eyes_max_fdist < -epsilon)
	{
		if (pNode->negChild != -1) 
			TraverseBsp(pNode->negChild, pEyes, tBox2, pAction, param);
		return;
	}

	float eyesmin_div_deltadist = (float)(eyes_min_fdist / (eyes_min_fdist - eyes_max_fdist));
	AjustDelta(pEyes, (float*)&ajusted.x, eyesmin_div_deltadist);
				
	if (eyes_min_fdist <= 0.0)
	{
		if (pNode->negChild != -1)
		{
			MergeBox(newEyes, &pEyes[0][0], (float*)&ajusted.x);
			TraverseBsp(pNode->negChild, newEyes, tBox2, pAction, param);
		}
		if (pNode->posChild != -1)
		{
			MergeBox(newEyes, (float*)&ajusted.x, &pEyes[1][0]);
			TraverseBsp(pNode->posChild, newEyes, tBox1, pAction, param);
		}
	}
	else
	{
		if (pNode->posChild != (short)-1)
		{
			MergeBox(newEyes, &pEyes[0][0], (float*)&ajusted.x);
			TraverseBsp(pNode->posChild, newEyes, tBox1, pAction, param);
		}
		if (pNode->negChild != (short)-1)
		{
			MergeBox(newEyes, (float*)&ajusted.x, &pEyes[1][0]);
			TraverseBsp(pNode->negChild, newEyes, tBox2, pAction, param);
		}
	}
}

bool CWMO_Group::Delete()
{
	return false;
}


void CWMO_Group::FixColors(CBgra* mocv, uint32 mocv_count, const SWMO_Group_BatchDef* moba)
{
	int32_t intBatchStart = 0;

	if (m_GroupHeader.transBatchCount > 0)
		intBatchStart = moba[(uint16)m_GroupHeader.transBatchCount].vertexStart;

	if (mocv_count > 0)
	{
		for (int32_t i = 0; i < mocv_count; i++)
		{
			auto& color = mocv[i];

			// Int / ext batches
			if (i >= intBatchStart)
			{
				int32_t r = (color.r + (color.a * color.r / 64.0f)) / 2.0f;
				int32_t g = (color.g + (color.a * color.g / 64.0f)) / 2.0f;
				int32_t b = (color.b + (color.a * color.b / 64.0f)) / 2.0f;

				color.r = glm::min(r, 255);
				color.g = glm::min(g, 255);
				color.b = glm::min(b, 255);
				color.a = 255u;
			}
			else
			{
				color.r /= 2;
				color.g /= 2;
				color.b /= 2;
			}
		}
	}
}


/*
void CWMO_Group::FixColors(CBgra* mocv, uint32 mocv_count, const SWMO_Group_BatchDef* moba)
{
	uint32 begin_second_fixup = 0;
	if (m_GroupHeader.transBatchCount)
	{
		begin_second_fixup = moba[(uint16)m_GroupHeader.transBatchCount].vertexStart + 1;
	}

	if (m_WMOModel.GetHeader().flags.lighten_interiors)
	{
		for (uint32 i = begin_second_fixup; i < mocv_count; ++i)
		{
			mocv[i].a = m_GroupHeader.flags.IS_OUTDOOR ? 255 : 0;
		}
	}
	else
	{
		uint8 r = 0, g = 0, b = 0;

		if (m_WMOModel.GetHeader().flags.skip_base_color == 0)
		{
			r = m_WMOModel.GetHeader().ambColor.r;
			g = m_WMOModel.GetHeader().ambColor.g;
			b = m_WMOModel.GetHeader().ambColor.b;
		}

		for (uint32 mocv_index = 0; mocv_index < begin_second_fixup; ++mocv_index)
		{
			mocv[mocv_index].b -= b;
			mocv[mocv_index].g -= g;
			mocv[mocv_index].r -= r;

			float v38 = (float)mocv[mocv_index].a / 255.0f;

			float v11 = (float)mocv[mocv_index].b - v38 * (float)mocv[mocv_index].b;
			_ASSERT(v11 > -0.5f);
			_ASSERT(v11 < 255.5f);
			mocv[mocv_index].b = v11 / 2;

			float v13 = (float)mocv[mocv_index].g - v38 * (float)mocv[mocv_index].g;
			_ASSERT(v13 > -0.5f);
			_ASSERT(v13 < 255.5f);
			mocv[mocv_index].g = v13 / 2;

			float v14 = (float)mocv[mocv_index].r - v38 * (float)mocv[mocv_index].r;
			_ASSERT(v14 > -0.5f);
			_ASSERT(v14 < 255.5f);
			mocv[mocv_index].r = v14 / 2;
		}

		for (uint32 i = begin_second_fixup; i < mocv_count; ++i)
		{
			int32 v19 = (mocv[i].b * mocv[i].a) / 64 + mocv[i].b - b;
			mocv[i].b = std::min(255, std::max(v19 / 2, 0));

			int32 v30 = (mocv[i].g * mocv[i].a) / 64 + mocv[i].g - g;
			mocv[i].g = std::min(255, std::max(v30 / 2, 0));

			int32 v33 = (mocv[i].a * mocv[i].r) / 64 + mocv[i].r - r;
			mocv[i].r = std::min(255, std::max(v33 / 2, 0));

			mocv[i].a = m_GroupHeader.flags.IS_OUTDOOR ? 0xFF : 0x00;
		}
	}
}*/

#endif
