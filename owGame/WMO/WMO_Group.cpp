#include "stdafx.h"

// Include
#include "WMO.h"
#include "WMO_Group_Instance.h"

// General
#include "WMO_Group.h"

// Additional 
#include "WMO_Doodad_Instance.h"
#include "WMO_Liquid_Instance.h"
#include "WMO_Fixes.h"

WMO_Group::WMO_Group(IBaseManager* BaseManager, IRenderDevice& RenderDevice, const CWMO& WMOModel, const uint32 _groupIndex, std::string _groupName, std::shared_ptr<IFile> _groupFile)
	: m_GroupName(_groupName)
	, m_GroupIndex(_groupIndex)
	, m_F(_groupFile)
	, m_IsMOCVExists(false)
	, m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_WMOModel(WMOModel)
{
	m_WMOLiqiud = nullptr;
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
		const SWMO_Doodad_PlacementInfo& placement = m_ParentWMO.lock()->m_DoodadsPlacementInfos[index];

		CWMO_Doodad_Instance* inst = _parent.lock()->CreateSceneNode<CWMO_Doodad_Instance>(m_ParentWMO.lock()->m_DoodadsFilenames + placement.flags.nameIndex, weak_from_this(), index);
        inst->Initialize(placement);
		m_BaseManager->GetManager<ILoader>()->AddToLoadQueue(inst);
		_parent.lock()->addDoodadInstance(inst);
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

	// Read file
	char fourcc[5];
	uint32 size = 0;
	while (!m_F->isEof())
	{
		memset(fourcc, 0, 4);
		size = 0;
		m_F->readBytes(fourcc, 4);
		m_F->readBytes(&size, 4);
		flipcc(fourcc);
		fourcc[4] = 0;
		if (size == 0)	continue;
		uint32_t nextpos = m_F->getPos() + size;

		if (strcmp(fourcc, "MVER") == 0)
		{
			uint32 version;
			m_F->readBytes(&version, 4);
			_ASSERT(version == 17);
		}
		else if (strcmp(fourcc, "MOGP") == 0)
		{
			// The MOGP chunk size will be way more than the header variables!
			nextpos = m_F->getPos() + sizeof(SWMO_Group_HeaderDef);

			m_F->readBytes(&m_Header, sizeof(SWMO_Group_HeaderDef));

			_ASSERT(m_Header.flags.HAS_3_MOTV == 0);

			// Bounds
			m_Bounds.set(m_Header.boundingBox.min, m_Header.boundingBox.max, true);
		}
		else if (strcmp(fourcc, "MOPY") == 0) // Material info for triangles
		{
			uint32 materialsInfoCount = size / sizeof(SWMO_Group_MaterialDef);
			SWMO_Group_MaterialDef* materialsInfo = (SWMO_Group_MaterialDef*)m_F->getDataFromCurrent();
			for (uint32 i = 0; i < materialsInfoCount; i++)
			{
				m_MaterialsInfo.push_back(materialsInfo[i]);
			}
		}
		else if (strcmp(fourcc, "MOVI") == 0) // Indices
		{
			_ASSERT(IB_Default == nullptr);
			uint32 indicesCount = size / sizeof(uint16);
			uint16* indices = (uint16*)m_F->getDataFromCurrent();
			// Buffer
			IB_Default = m_RenderDevice.GetObjectsFactory().CreateIndexBuffer(indices, indicesCount);

			dataFromMOVI = indices;
		}
		else if (strcmp(fourcc, "MOVT") == 0) // Vertices chunk.
		{
			_ASSERT(VB_Vertexes == nullptr);
			uint32 vertexesCount = size / sizeof(vec3);
			vec3* vertexes = (vec3*)m_F->getDataFromCurrent();
			// Convert
			for (uint32 i = 0; i < vertexesCount; i++)
			{
				vertexes[i] = Fix_XZmY(vertexes[i]);
			}
			// Buffer
			VB_Vertexes = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(vertexes, vertexesCount);
			VB_Colors = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(vertexes, vertexesCount);
			//m_Bounds.calculate(vertexes, vertexesCount);

			dataFromMOVT = vertexes;
		}
		else if (strcmp(fourcc, "MONR") == 0) // Normals
		{
			_ASSERT(VB_Normals == nullptr);
			uint32 normalsCount = size / sizeof(vec3);
			vec3* normals = (vec3*)m_F->getDataFromCurrent();
			// Convert
			for (uint32 i = 0; i < normalsCount; i++)
			{
				normals[i] = Fix_XZmY(normals[i]);
			}
			// Buffer
			VB_Normals = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(normals, normalsCount);
		}
		else if (strcmp(fourcc, "MOTV") == 0) // Texture coordinates
		{
			uint32 textureCoordsCount = size / sizeof(vec2);
			vec2* textureCoords = (vec2*)m_F->getDataFromCurrent();
			VB_TextureCoords.push_back(m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(textureCoords, textureCoordsCount));
			VB_TextureCoords.push_back(m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(textureCoords, textureCoordsCount));
		}
		else if (strcmp(fourcc, "MOBA") == 0) // WMO_Group_Batch
		{
			// Temp code
			uint32 batchesCount = size / sizeof(SWMO_Group_BatchDef);
			SWMO_Group_BatchDef* batches = (SWMO_Group_BatchDef*)m_F->getDataFromCurrent();
			moba = new SWMO_Group_BatchDef[batchesCount];
			memcpy(moba, batches, sizeof(SWMO_Group_BatchDef) * batchesCount);

			for (uint32 i = 0; i < size / sizeof(SWMO_Group_BatchDef); i++)
			{
				SWMO_Group_BatchDef batchDef;
				m_F->readBytes(&batchDef, sizeof(SWMO_Group_BatchDef));
				m_WMOBatchs.push_back(batchDef);
			}
		}
		else if (strcmp(fourcc, "MOLR") == 0) // Light references
		{
			_ASSERT(m_Header.flags.HAS_LIGHTS);
			uint32 lightsIndexesCount = size / sizeof(uint16);
			uint16* lightsIndexes = (uint16*)m_F->getDataFromCurrent();
			for (uint32 i = 0; i < lightsIndexesCount; i++)
			{
				m_WMOLightsIndexes.push_back(lightsIndexes[i]);
			}
		}
		else if (strcmp(fourcc, "MODR") == 0) // Doodad references
		{
			_ASSERT(m_Header.flags.HAS_DOODADS);
			uint32 doodadsIndexesCount = size / sizeof(uint16);
			uint16* doodadsIndexes = (uint16*)m_F->getDataFromCurrent();
			for (uint32 i = 0; i < doodadsIndexesCount; i++)
			{
				uint16 index = doodadsIndexes[i];
				m_DoodadsPlacementIndexes.push_back(index);
			}
		}
		else if (strcmp(fourcc, "MOBN") == 0)
		{
			_ASSERT(m_Header.flags.HAS_COLLISION);

			collisionCount = size / sizeof(SWMO_Group_MOBNDef);
			collisions = (SWMO_Group_MOBNDef*)m_F->getDataFromCurrent();
		}
		else if (strcmp(fourcc, "MOBR") == 0)
		{
			uint32 indexesCnt = size / sizeof(uint16);
			uint16* indices = (uint16*)m_F->getDataFromCurrent();

			/*collisionIndexes.reserve(indexesCnt * 3);
			for (uint32 i = 0; i < indexesCnt; i++)
			{
				collisionIndexes[i * 3 + 0] = dataFromMOVI[3 * indices[i] + 0];
				collisionIndexes[i * 3 + 1] = dataFromMOVI[3 * indices[i] + 1];
				collisionIndexes[i * 3 + 2] = dataFromMOVI[3 * indices[i] + 2];
			}*/
		}
		else if (strcmp(fourcc, "MOCV") == 0) // Vertex colors
		{
			_ASSERT(m_Header.flags.HAS_VERTEX_COLORS);
			uint32 vertexColorsCount = size / sizeof(CBgra);
			CBgra* vertexColors = (CBgra*)m_F->getDataFromCurrent();
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
		else if (strcmp(fourcc, "MLIQ") == 0) // Liquid
		{
			m_F->readBytes(&m_LiquidHeader, sizeof(SWMO_Group_MLIQDef));

			Log::Green("WMO[%s]: LiquidType CHUNK = HEADER[%d] [%d]", m_WMOModel.m_FileName.c_str(), m_Header.liquidType, m_WMOModel.m_Header.flags.use_liquid_type_dbc_id);

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

			//m_WMOLiqiud = std::make_shared<CWMO_Liquid>(m_LiquidHeader.A, m_LiquidHeader.B);
			//m_WMOLiqiud->CreateFromWMO(m_F, ParentWMO->m_Materials[m_LiquidHeader.materialID], DBC_LiquidType[liquid_type], m_Header.flags.IS_INDOOR);

		}
		else
		{
			Log::Fatal("WMO_Group[]: No implement group chunk %s [%d].", fourcc, size);
		}
		m_F->seek(nextpos);
	}

	for (uint32 i = 0; i < collisionCount; i++)
	{
		std::shared_ptr<CWMO_Group_Part_BSP_Node> collisionNode = std::make_shared<CWMO_Group_Part_BSP_Node>(shared_from_this(), collisions[i]);
		m_CollisionNodes.push_back(collisionNode);
	}

	m_F = nullptr;

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

void WMO_Group::initLighting()
{
	/*if (m_Header.flags.FLAG_IS_INDOOR && m_Header.flags.FLAG_HAS_VERTEX_COLORS)
	{
		vec3 dirmin(1, 1, 1);
		float lenmin;
		int lmin;
		for (uint32 i = 0; i < m_DoodadsIndexesCount; i++)
		{
			lenmin = 999999.0f * 999999.0f;
			lmin = 0;
			DoodadInstance* mi = m_ParentWMO->m_M2Instances[m_PlacementInfoIndexes[i]];
			for (uint32 j = 0; j < m_ParentWMO->m_Header.nLights; j++)
			{
				WMO_Part_Light* l = m_ParentWMO->m_Lights[j];
				vec3 dir = l->lightDef.pos - mi->placementInfo->position;
				float ll = dir.length2();
				if (ll < lenmin)
				{
					lenmin = ll;
					dirmin = dir;
					lmin = j;
				}
			}
			mi->light = lmin;
			mi->ldir = dirmin;
		}
		m_EnableOutdoorLights = false;
	}
	else
	{
		m_EnableOutdoorLights = true;
	}*/
}

//



void WMO_Group::Render(cmat4 _world) const
{
	/*{
		CWMO_GeomertyPass* pass = _Render->getTechniquesMgr()->WMO_Pass.operator->();
		pass->Bind();
		{
			pass->setWorld(_world);

			// Vertex colors
			pass->SetHasMOCV(m_IsMOCVExists && m_Quality.WMO_MOCV);

			// Ambient color
			pass->SetUseAmbColor(m_Quality.WMO_AmbColor);
			//pass->SetAmbColor(m_ParentWMO->m_Header.getAmbColor());

			for (auto it : m_WMOBatchIndexes)
			{
				it->Render();
			}
		}
		pass->Unbind();
	}*/

	//RenderCollision(_world);
}

void WMO_Group::RenderCollision(cmat4 _world) const
{
	/*for (auto& node : m_CollisionNodes)
	{
		node->Render(_world);
	}*/
}
