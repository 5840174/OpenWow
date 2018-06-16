#include "stdafx.h"

// General
#include "liquid.h"

Liquid::Liquid(uint32 x, uint32 y) :
	m_TilesX(x),
	m_TilesY(y),
	ydir(1.0f),
	m_SkyManager(GetManager<ISkyManager>())
{
	m_TilesCount = (m_TilesX + 1) * (m_TilesY + 1);
}

//--

void Liquid::Render(cmat4 _worldMatrix)
{
	_Render->r.setBlendMode(true, R_BlendFunc::BS_BLEND_SRC_ALPHA, R_BlendFunc::BS_BLEND_INV_SRC_ALPHA);
	_Render->r.setCullMode(R_CullMode::RS_CULL_NONE);
	_Render->r.setDepthTest(true);
	_Render->r.setDepthMask(true);

	_Render->r.setGeometry(__geom);

	if (m_Type == DBC_LIQUIDTYPE_Type::lq_river || m_Type == DBC_LIQUIDTYPE_Type::lq_ocean)
	{
		_Render->getTechniquesMgr()->m_Water->Bind();
		_Render->getTechniquesMgr()->m_Water->SetWorldMatrix(_worldMatrix);
	}
	else
	{
		_Render->getTechniquesMgr()->m_Magma->Bind();
		_Render->getTechniquesMgr()->m_Water->SetWorldMatrix(_worldMatrix);
	}

	uint32_t texidx = (uint32_t)(/*_World->EnvM()->animtime*/ 0.0f / 60.0f) % m_Textures.size();
	_Render->r.setTexture(10, m_Textures[texidx], 0, 0);

	if (m_Type == DBC_LIQUIDTYPE_Type::lq_river || m_Type == DBC_LIQUIDTYPE_Type::lq_ocean)
	{
		if (m_SkyManager != nullptr)
		{
			_Render->getTechniquesMgr()->m_Water->SetWaterColorLight(m_SkyManager->GetColor(LIGHT_COLOR_RIVER_LIGHT));
			_Render->getTechniquesMgr()->m_Water->SetWaterColorDark(m_SkyManager->GetColor(LIGHT_COLOR_RIVER_DARK));
			_Render->getTechniquesMgr()->m_Water->SetShallowAlpha(m_SkyManager->GetWaterShallowAlpha());
			_Render->getTechniquesMgr()->m_Water->SetDeepAlpha(m_SkyManager->GetWaterDarkAlpha());
		}
		else
		{
			_Render->getTechniquesMgr()->m_Water->SetWaterColorLight(vec3(0.0f, 0.0f, 1.0f));
			_Render->getTechniquesMgr()->m_Water->SetWaterColorDark(vec3(0.0f, 0.0f, 1.0f));
			_Render->getTechniquesMgr()->m_Water->SetShallowAlpha(1.0f);
			_Render->getTechniquesMgr()->m_Water->SetDeepAlpha(1.0f);
		}
	}

	_Render->r.drawIndexed(PRIM_TRILIST, 0, m_IndicesCnt, 0, m_VerticesCnt, false);

	PERF_INC(PERF_MAP_CHUNK_MH20);

	if (m_Type == DBC_LIQUIDTYPE_Type::lq_river || m_Type == DBC_LIQUIDTYPE_Type::lq_ocean)
	{
		_Render->getTechniquesMgr()->m_Water->Unbind();
	}
	else
	{
		_Render->getTechniquesMgr()->m_Magma->Unbind();
	}

	_Render->r.setCullMode(R_CullMode::RS_CULL_NONE);
}

//--

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

void Liquid::initGeometry(DBC_LIQUIDTYPE_Type _type, IFile* f)
{
	m_Type = _type;

	SLiquidVertex* map = (SLiquidVertex*)(f->GetDataFromCurrent());
	SLiquidFlag* flags = (SLiquidFlag*)(f->GetDataFromCurrent() + m_TilesCount * sizeof(SLiquidVertex));

	layer.x = 0;
	layer.y = 0;
	layer.Width = m_TilesX;
	layer.Height = m_TilesY;

	for (uint32 j = 0; j < m_TilesY + 1; j++)
	{
		for (uint32 i = 0; i < m_TilesX + 1; i++)
		{
			uint32 p = j * (m_TilesX + 1) + i;

			if (flags[p].liquid & 0x08)
			{
				layer.renderTiles.push_back(false);
			}
			else
			{
				flag = flags[p].liquid;
				layer.renderTiles.push_back(true);
			}

			if (_type == DBC_LIQUIDTYPE_Type::lq_river)
			{
				layer.heights.push_back(map[p].waterVert.height);
				layer.depths.push_back(map[p].waterVert.depth);
			}
			else if (_type == DBC_LIQUIDTYPE_Type::lq_ocean)
			{
				layer.heights.push_back(0.0f);
				layer.depths.push_back(map[p].oceanVert.depth);
			}
			else if (_type == DBC_LIQUIDTYPE_Type::lq_magma)
			{
				layer.heights.push_back(map[p].magmaVert.height);
				layer.textureCoords.push_back
				(
					make_pair
					(
						static_cast<float>(map[p].magmaVert.s) / 1000.0f,
						static_cast<float>(map[p].magmaVert.t) / 1000.0f
					)
				);
			}
			else if (_type == DBC_LIQUIDTYPE_Type::lq_slime)
			{
				layer.heights.push_back(map[p].magmaVert.height);
				layer.textureCoords.push_back
				(
					make_pair
					(
						static_cast<float>(map[p].magmaVert.s) / 1000.0f,
						static_cast<float>(map[p].magmaVert.t) / 1000.0f
					)
				);
			}
			else
			{
				fail1();
			}
		}
	}

	createBuffer();
}

#pragma region Types
#include __PACK_BEGIN
struct SLiquidVertexData
{
	vec3 position;
	vec3 textureCoord;
	vec3 normal;
};
#include __PACK_END
#pragma endregion

void Liquid::createBuffer()
{
	vector<SLiquidVertexData> mh2oVertices;
	vector<uint16> m_Indices;

	uint32 cntr = 0;


	for (uint8 y = layer.y; y < layer.Height + layer.y; y++)
	{
		for (uint8 x = layer.x; x < layer.Width + layer.x; x++)
		{
			unsigned tx = x - layer.x;
			unsigned ty = y - layer.y;

			// p1--p4
			// |    |
			// p2--p3
			unsigned p1 = tx + ty * (layer.Width + 1);
			unsigned p2 = tx + (ty + 1)     * (layer.Width + 1);
			unsigned p3 = tx + 1 + (ty + 1) * (layer.Width + 1);
			unsigned p4 = tx + 1 + ty * (layer.Width + 1);

			// heights
			float h1, h2, h3, h4;
			h1 = h2 = h3 = h4 = 0.0f;
			if (layer.heights.size() > 0)
			{
				h1 = layer.heights[p1];
				h2 = layer.heights[p2];
				h3 = layer.heights[p3];
				h4 = layer.heights[p4];
			}

			// R_Texture coords
			std::pair<float, float> t1, t2, t3, t4;
			t1 = make_pair(0.0f, 0.0f);
			t2 = make_pair(0.0f, 1.0f);
			t3 = make_pair(1.0f, 1.0f);
			t4 = make_pair(1.0f, 0.0f);
			if (layer.textureCoords.size() > 0)
			{
				t1 = layer.textureCoords[p1];
				t2 = layer.textureCoords[p2];
				t3 = layer.textureCoords[p3];
				t4 = layer.textureCoords[p4];
			}

			// alpha
			float a1, a2, a3, a4;
			a1 = a2 = a3 = a4 = 1.0f;
			if (layer.depths.size() > 0)
			{
				a1 = minf(static_cast<float>(layer.depths[p1]) / 127.0f, 1.0f); // whats the magic formular here ???
				a2 = minf(static_cast<float>(layer.depths[p2]) / 127.0f, 1.0f);
				a3 = minf(static_cast<float>(layer.depths[p3]) / 127.0f, 1.0f);
				a4 = minf(static_cast<float>(layer.depths[p4]) / 127.0f, 1.0f);
			}

			// Skip hidden water tile
			if (layer.renderTiles.size() != 0)
			{
				if (!layer.renderTiles[tx + ty * layer.Width])
				{
					continue;
				}
			}

			// Insert vertex

			mh2oVertices.push_back
			({
				vec3(C_UnitSize * static_cast<float>(x), h1, ydir * (C_UnitSize * static_cast<float>(y))),
				vec3(t1.first, t1.second, a1),
				defaultNormal
				});

			mh2oVertices.push_back
			({
				vec3(C_UnitSize * static_cast<float>(x), h2, ydir * (C_UnitSize + C_UnitSize * static_cast<float>(y))),
				vec3(t2.first, t2.second, a2),
				defaultNormal
				});

			mh2oVertices.push_back
			({
				vec3(C_UnitSize + C_UnitSize * static_cast<float>(x), h4, ydir * (C_UnitSize * static_cast<float>(y))),
				vec3(t4.first, t4.second, a4),
				defaultNormal
				});

			mh2oVertices.push_back
			({
				vec3(C_UnitSize + C_UnitSize * static_cast<float>(x), h3, ydir * (C_UnitSize + C_UnitSize * static_cast<float>(y))),
				vec3(t3.first, t3.second, a3),
				defaultNormal
				});

			m_Indices.push_back(cntr + 2);
			m_Indices.push_back(cntr + 1);
			m_Indices.push_back(cntr + 0);
			m_Indices.push_back(cntr + 3);
			m_Indices.push_back(cntr + 1);
			m_Indices.push_back(cntr + 2);

			cntr += 4;
		}
	}

	// Vertex buffer
	R_Buffer* __vb = _Render->r.createVertexBuffer(mh2oVertices.size() * sizeof(SLiquidVertexData), mh2oVertices.data(), false);
	m_VerticesCnt = static_cast<uint32>(mh2oVertices.size());
	assert1(m_VerticesCnt > 0);

	// Index bufer
	R_Buffer* __ib = _Render->r.createIndexBuffer(m_Indices.size() * sizeof(uint16), m_Indices.data(), false);
	m_IndicesCnt = static_cast<uint32>(m_Indices.size());
	assert1(m_IndicesCnt > 0);

	// Geometry
	__geom = _Render->r.beginCreatingGeometry(_Render->getRenderStorage()->__layoutWater);
	__geom->setGeomVertexParams(__vb, R_DataType::T_FLOAT, 0, sizeof(SLiquidVertexData));
	__geom->setGeomVertexParams(__vb, R_DataType::T_FLOAT, 12, sizeof(SLiquidVertexData));
	__geom->setGeomVertexParams(__vb, R_DataType::T_FLOAT, 24, sizeof(SLiquidVertexData));
	__geom->setGeomIndexParams(__ib, R_IndexFormat::IDXFMT_16);
	__geom->finishCreatingGeometry();
}

void Liquid::InitTextures(DBC_LIQUIDTYPE_Type _liquidType)
{
	string baseName;

	//if ((flag & 0x08) == 0)
	//	Log::Warn("[%d][%d][%d][%d][%d]", flag & 0x01, flag & 0x02, flag & 0x04, flag & 0x10, flag & 0x20);

	if (_liquidType == DBC_LIQUIDTYPE_Type::lq_river)
	{
		baseName = "XTextures\\river\\lake_a";
	}
	else if (_liquidType == DBC_LIQUIDTYPE_Type::lq_ocean)
	{
		baseName = "XTextures\\ocean\\ocean";
	}
	else if (_liquidType == DBC_LIQUIDTYPE_Type::lq_magma)
	{
		baseName = "XTextures\\lava\\lava";
	}
	else if (_liquidType == DBC_LIQUIDTYPE_Type::lq_slime)
	{
		baseName = "XTextures\\slime\\slime";
	}
	else
	{
		fail1();
	}

	char buf[256];
	for (int i = 1; i <= 30; i++)
	{
		sprintf(buf, "%s.%d.blp", baseName.c_str(), i);
		SmartTexturePtr texture = _Render->TexturesMgr()->Add(buf);
		m_Textures.push_back(texture);
	}
}
