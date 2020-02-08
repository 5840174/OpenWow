#include "stdafx.h"

// General
#include "liquid.h"


CLiquid::CLiquid(IRenderDevice& RenderDevice, uint32 x, uint32 y)
	: m_TilesX(x)
	, m_TilesY(y)
	, ydir(1.0f)
	, m_RenderDevice(RenderDevice)
{
	m_TilesCount = (m_TilesX + 1) * (m_TilesY + 1);
}

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

void CLiquid::createLayers(std::shared_ptr<const DBC_LiquidTypeRecord> _type, std::shared_ptr<IFile> f)
{
	SLiquidVertex* map = (SLiquidVertex*)(f->getDataFromCurrent());
	SLiquidFlag* flags = (SLiquidFlag*)(f->getDataFromCurrent() + m_TilesCount * sizeof(SLiquidVertex));

	std::shared_ptr<CLiquidLayer> layer = std::make_shared<CLiquidLayer>(m_RenderDevice, m_RenderDevice.GetObjectsFactory().CreateModel());
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

	m_WaterLayers.push_back(layer);

	createBuffer();
}

#pragma region Types
#include __PACK_BEGIN
struct SLiquidVertexData
{
	vec3 position;
	vec3 textureCoord;
};
#include __PACK_END
#pragma endregion

void CLiquid::createBuffer()
{
	for (auto& layer : m_WaterLayers)
	{
		std::vector<vec3> mh2oVerticesPos;
		std::vector<vec3> mh2oVerticesTex;
		std::vector<uint16> m_Indices;
		uint32 cntr = 0;

		for (uint8 y = layer->y; y < layer->Height + layer->y; y++)
		{
			for (uint8 x = layer->x; x < layer->Width + layer->x; x++)
			{
				unsigned tx = x - layer->x;
				unsigned ty = y - layer->y;

				// p1--p4
				// |    |
				// p2--p3
				unsigned p1 = tx + ty * (layer->Width + 1);
				unsigned p2 = tx + (ty + 1)     * (layer->Width + 1);
				unsigned p3 = tx + 1 + (ty + 1) * (layer->Width + 1);
				unsigned p4 = tx + 1 + ty * (layer->Width + 1);

				// heights
				float h1, h2, h3, h4;
				h1 = h2 = h3 = h4 = 0.0f;
				if (layer->heights.size() > 0)
				{
					h1 = layer->heights[p1];
					h2 = layer->heights[p2];
					h3 = layer->heights[p3];
					h4 = layer->heights[p4];
				}

				// R_Texture coords
				std::pair<float, float> t1, t2, t3, t4;
				t1 = std::make_pair(0.0f, 0.0f);
				t2 = std::make_pair(0.0f, 1.0f);
				t3 = std::make_pair(1.0f, 1.0f);
				t4 = std::make_pair(1.0f, 0.0f);
				if (layer->textureCoords.size() > 0)
				{
					t1 = layer->textureCoords[p1];
					t2 = layer->textureCoords[p2];
					t3 = layer->textureCoords[p3];
					t4 = layer->textureCoords[p4];
				}

				// alpha
				float a1, a2, a3, a4;
				a1 = a2 = a3 = a4 = 1.0f;
				if (layer->depths.size() > 0)
				{
					a1 = minf(static_cast<float>(layer->depths[p1]) / 127.0f, 1.0f); // whats the magic formular here ???
					a2 = minf(static_cast<float>(layer->depths[p2]) / 127.0f, 1.0f);
					a3 = minf(static_cast<float>(layer->depths[p3]) / 127.0f, 1.0f);
					a4 = minf(static_cast<float>(layer->depths[p4]) / 127.0f, 1.0f);
				}

				// Skip hidden water tile
				if (layer->renderTiles.size() != 0)
				{
					if (!layer->renderTiles[tx + ty * layer->Width])
					{
						continue;
					}
				}

				// Insert vertex

				mh2oVerticesPos.push_back(vec3(C_UnitSize * static_cast<float>(x), h1, ydir * (C_UnitSize * static_cast<float>(y))));
				mh2oVerticesTex.push_back(vec3(t1.first, t1.second, a1));

				mh2oVerticesPos.push_back(vec3(C_UnitSize * static_cast<float>(x), h2, ydir * (C_UnitSize + C_UnitSize * static_cast<float>(y))));
				mh2oVerticesTex.push_back(vec3(t2.first, t2.second, a2));

				mh2oVerticesPos.push_back(vec3(C_UnitSize + C_UnitSize * static_cast<float>(x), h4, ydir * (C_UnitSize * static_cast<float>(y))));
				mh2oVerticesTex.push_back(vec3(t4.first, t4.second, a4));

				mh2oVerticesPos.push_back(vec3(C_UnitSize + C_UnitSize * static_cast<float>(x), h3, ydir * (C_UnitSize + C_UnitSize * static_cast<float>(y))));
				mh2oVerticesTex.push_back(vec3(t3.first, t3.second, a3));

				m_Indices.push_back(cntr + 2);
				m_Indices.push_back(cntr + 1);
				m_Indices.push_back(cntr + 0);
				m_Indices.push_back(cntr + 3);
				m_Indices.push_back(cntr + 1);
				m_Indices.push_back(cntr + 2);
				cntr += 4;
			}
		}

		std::shared_ptr<IGeometry> geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
		geometry->AddVertexBuffer(BufferBinding("POSITION", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(mh2oVerticesPos));
		geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(mh2oVerticesTex));
		geometry->SetIndexBuffer(m_RenderDevice.GetObjectsFactory().CreateIndexBuffer(m_Indices));

		layer->AddConnection(layer->GetMaterial(), geometry);
	}
}
