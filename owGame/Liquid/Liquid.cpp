#include "stdafx.h"

// General
#include "liquid.h"


CLiquid::CLiquid(IRenderDevice& RenderDevice, uint32 x, uint32 y)
	: m_TilesX(x)
	, m_TilesY(y)
	, ydir(1.0f)
	, m_RenderDevice(RenderDevice)
{}

CLiquid::~CLiquid()
{
}

#pragma region Types
#include __PACK_BEGIN

struct SLiquidVertex
{
	union
	{
		struct SWaterVert
		{
			int8 depth;
			int8 flow0Pct;
			int8 flow1Pct;
			int8 unk0;
			float height;
		} waterVert;

		struct SOceanVert
		{
			uint8_t depth;
			uint8_t foam;
			uint8_t wet;
			uint8_t unk0;
		} oceanVert;

		struct SMagmaVert
		{
			uint16 s;
			uint16 t;
			float height;
		} magmaVert;
	};
};

struct SLiquidFlag
{
	uint8 liquid : 6;    // 0x01 - 0x20
	uint8 fishable : 1;  // 0x40
	uint8 shared : 1;    // 0x80
};

#include __PACK_END
#pragma endregion

void CLiquid::createLayers(const DBC_LiquidTypeRecord* _type, const std::shared_ptr<IByteBuffer>& Bytes)
{
	SLiquidVertex* map = (SLiquidVertex*)(Bytes->getDataFromCurrent());
	SLiquidFlag* flags = (SLiquidFlag*)(Bytes->getDataFromCurrent() + ((m_TilesX + 1) * (m_TilesY + 1)) * sizeof(SLiquidVertex));

	std::shared_ptr<CLiquidLayer> layer = std::make_shared<CLiquidLayer>(m_RenderDevice);
	layer->LiquidType = _type;
	layer->InitTextures(_type->Get_Type());

	layer->x = 0;
	layer->y = 0;
	layer->Width = m_TilesX;
	layer->Height = m_TilesY;
	
    for (uint32 j = 0; j < m_TilesY + 1; j++)
    {
        for (uint32 i = 0; i < m_TilesX + 1; i++)
        {
            uint32 p = j * (m_TilesX + 1) + i;

            if (flags[p].liquid & 0x08)
            {
                layer->renderTiles.push_back(false);
            }
            else
            {
                layer->renderTiles.push_back(true);
            }

            layer->heights.push_back(map[p].magmaVert.height);
            //layer.depths.push_back((map[p].magmaVert.s / 255.0f) * 0.5f + 0.5f);
        }
    }


	/*for (uint32 j = 0; j < m_TilesY + 1; j++)
	{
		for (uint32 i = 0; i < m_TilesX + 1; i++)
		{
			uint32 p = j * (m_TilesX + 1) + i;

			layer->renderTiles.push_back((flags[p].liquid & 0x08) == 0);

			if (layer->VertexFormat == 0)
			{
				layer->heights.push_back(map[p].waterVert.height);
				layer->depths.push_back(map[p].waterVert.depth);
			}
			else if (layer->VertexFormat == 1)
			{
				layer->heights.push_back(map[p].magmaVert.height);
				layer->textureCoords.push_back
				(
					std::make_pair
					(
						static_cast<float>(map[p].magmaVert.s) / 8.0f,
						static_cast<float>(map[p].magmaVert.t) / 8.0f
					)
				);
			}
			else if (layer->VertexFormat == 2)
			{
				layer->depths.push_back(map[p].oceanVert.depth);
			}
		}
	}*/

	auto geometry = layer->CreateGeometry(ydir);
	layer->AddConnection(layer->GetMaterial(), geometry);

	m_WaterLayers.push_back(layer);
}
