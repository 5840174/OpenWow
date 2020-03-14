#include "stdafx.h"

// General
#include "LiquidLayer.h"

CLiquidLayer::CLiquidLayer(IRenderDevice& RenderDevice)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_RenderDevice(RenderDevice)
	, m_SkyManager(nullptr)
{
	//m_SkyManager = m_RenderDevice.GetBaseManager().GetManager<ISkyManager>();
	m_Material = std::make_shared<LiquidMaterial>(m_RenderDevice);
}

CLiquidLayer::~CLiquidLayer()
{
}

std::shared_ptr<IGeometry> CLiquidLayer::CreateGeometry(float YDir)
{
	std::vector<glm::vec3> mh2oVerticesPos;
	std::vector<glm::vec3> mh2oVerticesTex;
	std::vector<uint16> m_Indices;
	uint32 cntr = 0;

	for (uint8 y0 = y; y0 < Height + y; y0++)
	{
		for (uint8 x0 = x; x0 < Width + x; x0++)
		{
			unsigned tx = x0 - x;
			unsigned ty = y0 - y;

			// p1--p4
			// |    |
			// p2--p3
			unsigned p1 = tx + ty * (Width + 1);
			unsigned p2 = tx + (ty + 1)     * (Width + 1);
			unsigned p3 = tx + 1 + (ty + 1) * (Width + 1);
			unsigned p4 = tx + 1 + ty * (Width + 1);

			// heights
			float h1, h2, h3, h4;
			h1 = h2 = h3 = h4 = 0.0f;
			if (heights.size() > 0)
			{
				h1 = heights[p1];
				h2 = heights[p2];
				h3 = heights[p3];
				h4 = heights[p4];
			}

			// R_Texture coords
			std::pair<float, float> t1, t2, t3, t4;
			t1 = std::make_pair(0.0f, 0.0f);
			t2 = std::make_pair(0.0f, 1.0f);
			t3 = std::make_pair(1.0f, 1.0f);
			t4 = std::make_pair(1.0f, 0.0f);
			if (textureCoords.size() > 0)
			{
				t1 = textureCoords[p1];
				t2 = textureCoords[p2];
				t3 = textureCoords[p3];
				t4 = textureCoords[p4];
			}

			// alpha
			float a1, a2, a3, a4;
			a1 = a2 = a3 = a4 = 1.0f;
			if (depths.size() > 0)
			{
				a1 = minf(static_cast<float>(depths[p1]) / 127.0f, 1.0f); // whats the magic formular here ???
				a2 = minf(static_cast<float>(depths[p2]) / 127.0f, 1.0f);
				a3 = minf(static_cast<float>(depths[p3]) / 127.0f, 1.0f);
				a4 = minf(static_cast<float>(depths[p4]) / 127.0f, 1.0f);
			}

			// Skip hidden water tile
			if (renderTiles.size() != 0)
			{
				if (!renderTiles[tx + ty * Width])
				{
					continue;
				}
			}

			// Insert vertex

			mh2oVerticesPos.push_back(glm::vec3(C_UnitSize * static_cast<float>(x0), h1, YDir * (C_UnitSize * static_cast<float>(y0))));
			mh2oVerticesTex.push_back(glm::vec3(t1.first, t1.second, a1));

			mh2oVerticesPos.push_back(glm::vec3(C_UnitSize * static_cast<float>(x0), h2, YDir * (C_UnitSize + C_UnitSize * static_cast<float>(y0))));
			mh2oVerticesTex.push_back(glm::vec3(t2.first, t2.second, a2));

			mh2oVerticesPos.push_back(glm::vec3(C_UnitSize + C_UnitSize * static_cast<float>(x0), h4, YDir * (C_UnitSize * static_cast<float>(y0))));
			mh2oVerticesTex.push_back(glm::vec3(t4.first, t4.second, a4));

			mh2oVerticesPos.push_back(glm::vec3(C_UnitSize + C_UnitSize * static_cast<float>(x0), h3, YDir * (C_UnitSize + C_UnitSize * static_cast<float>(y0))));
			mh2oVerticesTex.push_back(glm::vec3(t3.first, t3.second, a3));

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

	return geometry;
}

std::shared_ptr<IMaterial> CLiquidLayer::GetMaterial() const
{
	return m_Material;
}



//
// IModel
//
bool CLiquidLayer::Render(const RenderEventArgs& renderEventArgs) const
{
	uint32_t texidx = (uint32_t)(renderEventArgs.TotalTime / 60.0f) % m_Textures.size();
	m_Material->SetTexture(0, m_Textures[texidx]);

	if (m_SkyManager != nullptr)
	{
		m_Material->SetColorLight(m_SkyManager->GetColor(LightColors::LIGHT_COLOR_RIVER_LIGHT));
		m_Material->SetColorDark(m_SkyManager->GetColor(LightColors::LIGHT_COLOR_RIVER_DARK));
		m_Material->SetShallowAlpha(m_SkyManager->GetWaterShallowAlpha());
		m_Material->SetDeepAlpha(m_SkyManager->GetWaterDarkAlpha());
	}
	else
	{
		m_Material->SetColorLight(vec3(0.0f, 0.0f, 1.0f));
		m_Material->SetColorDark(vec3(0.0f, 0.0f, 1.0f));
		m_Material->SetShallowAlpha(1.0f);
		m_Material->SetDeepAlpha(1.0f);
	}

	return ModelProxie::Render(renderEventArgs);
}



//
// Public
//
void CLiquidLayer::InitTextures(DBC_LIQUIDTYPE_Type::List _liquidType)
{
	std::string baseName;

	if (_liquidType == DBC_LIQUIDTYPE_Type::water)
	{
		baseName = "XTextures\\river\\lake_a";
	}
	else if (_liquidType == DBC_LIQUIDTYPE_Type::ocean)
	{
		baseName = "XTextures\\ocean\\ocean";
	}
	else if (_liquidType == DBC_LIQUIDTYPE_Type::magma)
	{
		baseName = "XTextures\\lava\\lava";
	}
	else if (_liquidType == DBC_LIQUIDTYPE_Type::slime)
	{
		baseName = "XTextures\\slime\\slime";
	}
	else
	{
		_ASSERT(false);
	}

	char buf[MAX_PATH];
	for (int i = 1; i <= 30; i++)
	{
		sprintf(buf, "%s.%d.blp", baseName.c_str(), i);
		std::shared_ptr<ITexture> texture = m_RenderDevice.GetObjectsFactory().LoadTexture2D(buf);
		m_Textures.push_back(texture);
	}
}

