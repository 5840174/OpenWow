#include "stdafx.h"

// General
#include "MapTileLiquid.h"

// Additional
#include "DBC/Tables/DBC_LiquidType.h"
#include "Liquid/LiquidModel.h"

#include __PACK_BEGIN

struct MH2O_Instance
{
	FOREIGN_KEY_ID(uint16, DBC_LiquidType, liquidType);
	uint16_t VertexFormat;

	float minHeightLevel;
	float maxHeightLevel;

	uint8 xOffset;
	uint8 yOffset;
	uint8 width;
	uint8 height;

	uint32 offsetExistsBitmap;
	uint32 offsetVertexData;
};

#include __PACK_END


CMapTileLiquid::CMapTileLiquid(IRenderDevice & RenderDevice, const std::shared_ptr<IByteBuffer>& Bytes, const MH2O_Header& header)
	: CLiquid(RenderDevice, 8, 8)
{
	for (uint32 j = 0; j < header.layersCount; j++)
	{
		const MH2O_Instance* mh2o_instance = (const MH2O_Instance*)(Bytes->getDataFromCurrent() + header.offsetInstances + sizeof(MH2O_Instance) * j);
		
		m_MinHeight = glm::min(m_MinHeight, mh2o_instance->minHeightLevel);
		m_MaxHeight = glm::max(m_MaxHeight, mh2o_instance->maxHeightLevel);

		const DBC_LiquidTypeRecord* liquidType = mh2o_instance->liquidType(RenderDevice.GetBaseManager().GetManager<CDBCStorage>());
		_ASSERT(liquidType != nullptr);

		SLiquidLayerDefinition layerDefinition;
		layerDefinition.TileStartX = mh2o_instance->xOffset;
		layerDefinition.TileStartY = mh2o_instance->yOffset;
		layerDefinition.TileWidth = mh2o_instance->width;
		layerDefinition.TileHeight = mh2o_instance->height;
		//layerDefinition.LiquidType = liquidType;
		//layerDefinition.InitTextures(waterLayer->LiquidType->Get_Type());
		//layerDefinition.VertexFormat = mh2o_instance->VertexFormat;
		//layerDefinition.MinHeightLevel = mh2o_instance->minHeightLevel;
		//layerDefinition.MaxHeightLevel = mh2o_instance->maxHeightLevel;
		layerDefinition.hasmask = mh2o_instance->offsetExistsBitmap != 0;
		if (layerDefinition.hasmask)
		{
			uint32 co = mh2o_instance->width * mh2o_instance->height / 8;
			if (mh2o_instance->width * mh2o_instance->height % 8 != 0)
				co++;

			memcpy(layerDefinition.mask, Bytes->getDataFromCurrent() + mh2o_instance->offsetExistsBitmap, co);
			for (uint32_t k = 0; k < (uint32_t)(layerDefinition.TileWidth * layerDefinition.TileHeight); k++)
				layerDefinition.renderTiles.push_back(getBitL2H(layerDefinition.mask, (uint32)k));
		}

		// Vertexes

		struct uv_map_entry
		{
			uint16 x; // divided by 8 for shaders
			uint16 y;
		};

		const uint32 vertexDataSize = (mh2o_instance->width + 1) * (mh2o_instance->height + 1);

		if (mh2o_instance->VertexFormat == 0)         // Case 0, Height and Depth data
		{
			const float* pHeights = (const float*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData);
			const uint8* pDepths = (const uint8*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData + (sizeof(float) * vertexDataSize));

			for (uint32 g = 0; g < vertexDataSize; g++)
			{
				layerDefinition.heights.push_back(pHeights[g]);
				layerDefinition.depths.push_back(pDepths[g]);
			}
		}
		else if (mh2o_instance->VertexFormat == 1)         // Case 1, Height and Texture Coordinate data
		{
			const float*      pHeights = (const float*)       (Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData);
			const uv_map_entry* pUVMap = (const uv_map_entry*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData + (sizeof(float) * vertexDataSize));

			for (uint32 g = 0; g < vertexDataSize; g++)
			{
				layerDefinition.heights.push_back(pHeights[g]);
				layerDefinition.textureCoords.push_back(std::make_pair(static_cast<float>(pUVMap[g].x) / 1024.0f * 8.0f, static_cast<float>(pUVMap[g].y) / 1024.0f * 8.0f));
			}
		}
		else if (mh2o_instance->VertexFormat == 2)         // Case 2, Depth only data (OCEAN)
		{
			const uint8* pDepths = (uint8*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData);
			for (uint32 g = 0; g < vertexDataSize; g++)
			{
				layerDefinition.depths.push_back(pDepths[g]);
			}
		}
		else if (mh2o_instance->VertexFormat == 3)         //Case 3, Height, Depth and R_Texture Coordinates
		{
			throw CException("Not implemented.");
			_ASSERT(false, "Vertex Format 3 is not supported now!");

			const float*      pHeights = (const float*)       (Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData);
			const uv_map_entry* pUVMap = (const uv_map_entry*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData + (sizeof(float) * vertexDataSize));
			const uint8*       pDepths = (const uint8*)       (Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData + (sizeof(float) * vertexDataSize) + (sizeof(uv_map_entry) * vertexDataSize));

			for (uint32 g = 0; g < vertexDataSize; g++)
			{
				layerDefinition.heights.push_back(pHeights[g]);
				layerDefinition.textureCoords.push_back(std::make_pair(static_cast<float>(pUVMap[g].x / 1024.0f * 8.0f), static_cast<float>(pUVMap[g].y / 1024.0f* 8.0f)));
				layerDefinition.depths.push_back(pDepths[g]);
			}
		}

		auto liquidModel = MakeShared(CLiquidModel, RenderDevice);
		liquidModel->Initialize(layerDefinition, liquidType);
		m_WaterLayers.push_back(liquidModel);
	}
}

CMapTileLiquid::~CMapTileLiquid()
{}
