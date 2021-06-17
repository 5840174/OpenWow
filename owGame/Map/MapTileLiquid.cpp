#include "stdafx.h"

// General
#include "MapTileLiquid.h"

// Additional
#include "DBC/Tables/DBC_LiquidType.h"
#include "Liquid/LiquidLayer.h"

#pragma region Types
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
#pragma endregion


CMapTileLiquid::CMapTileLiquid(IRenderDevice & RenderDevice, const std::shared_ptr<IByteBuffer>& Bytes, const MH2O_Header & header)
	: CLiquid(RenderDevice, 8, 8)
{
	for (uint32 j = 0; j < header.layersCount; j++)
	{
		MH2O_Instance* mh2o_instance = (MH2O_Instance*)(Bytes->getDataFromCurrent() + header.offsetInstances + sizeof(MH2O_Instance) * j);
		//assert1(mh2o_instance->minHeightLevel - mh2o_instance->maxHeightLevel > 0.001f);

		// Init liquid
		const DBC_LiquidTypeRecord* liquidType = mh2o_instance->liquidType(RenderDevice.GetBaseManager().GetManager<CDBCStorage>());
		_ASSERT(liquidType != nullptr);

		std::shared_ptr<CLiquidLayer> waterLayer = std::make_shared<CLiquidLayer>(RenderDevice);
		waterLayer->x = mh2o_instance->xOffset;
		waterLayer->y = mh2o_instance->yOffset;
		waterLayer->Width = mh2o_instance->width;
		waterLayer->Height = mh2o_instance->height;
		waterLayer->LiquidType = liquidType;
		waterLayer->InitTextures(waterLayer->LiquidType->Get_Type());
		waterLayer->VertexFormat = mh2o_instance->VertexFormat;
		waterLayer->MinHeightLevel = mh2o_instance->minHeightLevel;
		waterLayer->MaxHeightLevel = mh2o_instance->maxHeightLevel;
		waterLayer->hasmask = mh2o_instance->offsetExistsBitmap != 0;
		if (waterLayer->hasmask)
		{
			uint32 co = mh2o_instance->width * mh2o_instance->height / 8;
			if (mh2o_instance->width * mh2o_instance->height % 8 != 0)
			{
				co++;
			}
			memcpy(waterLayer->mask, Bytes->getDataFromCurrent() + mh2o_instance->offsetExistsBitmap, co);
			for (uint32_t k = 0; k < (uint32_t)(waterLayer->Width * waterLayer->Height); k++)
			{
				waterLayer->renderTiles.push_back(getBitL2H(waterLayer->mask, (uint32)k));
			}
		}

		// Vertexes

		struct uv_map_entry
		{
			uint16 x; // divided by 8 for shaders
			uint16 y;
		};

		const uint32 vertexDataSize = (mh2o_instance->width + 1) * (mh2o_instance->height + 1);

		if (waterLayer->VertexFormat == 0)         // Case 0, Height and Depth data
		{
			float* pHeights = (float*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData);
			uint8* pDepths = (uint8*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData + (sizeof(float) * vertexDataSize));

			for (uint32 g = 0; g < vertexDataSize; g++)
			{
				waterLayer->heights.push_back(pHeights[g]);
				waterLayer->depths.push_back(pDepths[g]);
			}
		}
		else if (waterLayer->VertexFormat == 1)         // Case 1, Height and Texture Coordinate data
		{
			float* pHeights = (float*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData);
			uv_map_entry* pUVMap = (uv_map_entry*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData + (sizeof(float) * vertexDataSize));

			for (uint32 g = 0; g < vertexDataSize; g++)
			{
				waterLayer->heights.push_back(pHeights[g]);
				waterLayer->textureCoords.push_back(std::make_pair(static_cast<float>(pUVMap[g].x) / 8.0f, static_cast<float>(pUVMap[g].y) / 8.0f));
			}
		}
		else if (waterLayer->VertexFormat == 2)         // Case 2, Depth only data (OCEAN)
		{
			uint8* pDepths = (uint8*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData);
			for (uint32 g = 0; g < vertexDataSize; g++)
			{
				waterLayer->depths.push_back(pDepths[g]);
			}
		}
		else if (waterLayer->VertexFormat == 3)         //Case 3, Height, Depth and R_Texture Coordinates
		{
			_ASSERT(false, "Vertex Format 3 is not supported now!");

			float* pHeights = (float*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData);
			uv_map_entry* pUVMap = (uv_map_entry*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData + (sizeof(float) * vertexDataSize));
			uint8* pDepths = (uint8*)(Bytes->getDataFromCurrent() + mh2o_instance->offsetVertexData + (sizeof(float) * vertexDataSize) + (sizeof(uv_map_entry) * vertexDataSize));

			for (uint32 g = 0; g < vertexDataSize; g++)
			{
				waterLayer->heights.push_back(pHeights[g]);
				waterLayer->textureCoords.push_back(std::make_pair(static_cast<float>(pUVMap[g].x / 8), static_cast<float>(pUVMap[g].y / 8)));
				waterLayer->depths.push_back(pDepths[g]);
			}
		}

		auto geometry = waterLayer->CreateGeometry(ydir);
		waterLayer->AddConnection(waterLayer->GetMaterial(), geometry);

		m_WaterLayers.push_back(waterLayer);
	}
}

CMapTileLiquid::~CMapTileLiquid()
{}
