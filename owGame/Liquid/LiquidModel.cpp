#include "stdafx.h"

// General
#include "LiquidModel.h"

// Additional
#include "Sky/SkyManager.h"

CLiquidModel::CLiquidModel(IRenderDevice& RenderDevice)
	: ModelProxie(RenderDevice.GetObjectsFactory().CreateModel())
	, m_RenderDevice(RenderDevice)
	, m_SkyManager(nullptr)
{
	m_SkyManager = m_RenderDevice.GetBaseManager().GetManager<CSkyManager>();
	m_Material = MakeShared(CLiquidMaterial, m_RenderDevice);
}

CLiquidModel::~CLiquidModel()
{
}

void CLiquidModel::Initialize(const SLiquidLayerDefinition& LiquidLayerDefinition, const DBC_LiquidTypeRecord* LiquidTypeRecord)
{
	auto liquidGeometry = CreateGeometryByDefinition(LiquidLayerDefinition);
	m_Material->InitializeTextures(LiquidTypeRecord);
	AddConnection(m_Material, liquidGeometry);

	m_LiquidType = LiquidTypeRecord->Get_Type();
}

std::shared_ptr<IMaterial> CLiquidModel::GetMaterial() const
{
	return m_Material;
}



//
// IModel
//
bool CLiquidModel::Render(const ShaderMap& Shaders) const
{
	m_Material->Update();

	if (m_SkyManager != nullptr)
	{
		if (m_LiquidType == DBC_LIQUIDTYPE_Type::water)
		{
			m_Material->SetColorLight(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_RIVER_LIGHT));
			m_Material->SetColorDark(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_RIVER_DARK));
			m_Material->SetShallowAlpha(m_SkyManager->GetWaterAlpha(ESkyWaterAlpha::SKY_WATER_SHALLOW));
			m_Material->SetDeepAlpha(m_SkyManager->GetWaterAlpha(ESkyWaterAlpha::SKY_WATER_DEEP));
		}
		else if (m_LiquidType == DBC_LIQUIDTYPE_Type::ocean)
		{
			m_Material->SetColorLight(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_OCEAN_LIGHT));
			m_Material->SetColorDark(m_SkyManager->GetColor(ESkyColors::SKY_COLOR_OCEAN_DARK));
			m_Material->SetShallowAlpha(m_SkyManager->GetWaterAlpha(ESkyWaterAlpha::SKY_OCEAN_SHALLOW));
			m_Material->SetDeepAlpha(m_SkyManager->GetWaterAlpha(ESkyWaterAlpha::SKY_OCEAN_DEEP));
		}
		else
		{
			// not needed for slime and magma
		}
	}
	else
	{
		m_Material->SetColorLight(ColorRGB(0.0f, 0.0f, 1.0f));
		m_Material->SetColorDark(ColorRGB(0.0f, 0.0f, 1.0f));
		m_Material->SetShallowAlpha(1.0f);
		m_Material->SetDeepAlpha(1.0f);
	}

	return ModelProxie::Render(Shaders);
}



//
// Private
//
std::shared_ptr<IGeometry> CLiquidModel::CreateGeometryByDefinition(const SLiquidLayerDefinition& LiquidLayerDefinition)
{
	std::vector<glm::vec3> verticesBuffer;
	std::vector<glm::vec3> textureCoordBuffer;
	std::vector<uint16> indicesBuffer;

	uint32 indexCntr = 0;
	for (uint8 y0 = LiquidLayerDefinition.TileStartY; y0 < LiquidLayerDefinition.TileStartY + LiquidLayerDefinition.TileHeight; y0++)
	{
		for (uint8 x0 = LiquidLayerDefinition.TileStartX; x0 < LiquidLayerDefinition.TileStartX + LiquidLayerDefinition.TileWidth; x0++)
		{
			unsigned tx = x0 - LiquidLayerDefinition.TileStartX;
			unsigned ty = y0 - LiquidLayerDefinition.TileStartY;

			// p1--p4
			// |    |
			// p2--p3
			unsigned p1 = tx + ty * (LiquidLayerDefinition.TileWidth + 1);
			unsigned p2 = tx + (ty + 1) * (LiquidLayerDefinition.TileWidth + 1);
			unsigned p3 = (tx + 1) + (ty + 1) * (LiquidLayerDefinition.TileWidth + 1);
			unsigned p4 = (tx + 1) + ty * (LiquidLayerDefinition.TileWidth + 1);

			// heights
			float h1, h2, h3, h4;
			h1 = h2 = h3 = h4 = 0.0f;
			if (false == LiquidLayerDefinition.heights.empty())
			{
				h1 = LiquidLayerDefinition.heights[p1];
				h2 = LiquidLayerDefinition.heights[p2];
				h3 = LiquidLayerDefinition.heights[p3];
				h4 = LiquidLayerDefinition.heights[p4];
			}

			// texture coords
			std::pair<float, float> t1, t2, t3, t4;
			const float cx = 1.0f / static_cast<float>(8) * 2.0f;
			const float cy = 1.0f / static_cast<float>(8) * 2.0f;
			t1 = std::make_pair(tx       * cx,  ty      * cy);
			t2 = std::make_pair(tx       * cx, (ty + 1) * cy);
			t3 = std::make_pair((tx + 1) * cx, (ty + 1) * cy);
			t4 = std::make_pair((tx + 1) * cx,  ty      * cy);
			if (false == LiquidLayerDefinition.textureCoords.empty())
			{
				t1 = LiquidLayerDefinition.textureCoords[p1];
				t2 = LiquidLayerDefinition.textureCoords[p2];
				t3 = LiquidLayerDefinition.textureCoords[p3];
				t4 = LiquidLayerDefinition.textureCoords[p4];
			}

			// alpha
			float a1, a2, a3, a4;
			a1 = a2 = a3 = a4 = 1.0f;
			if (false == LiquidLayerDefinition.depths.empty())
			{
				a1 = glm::min(static_cast<float>(LiquidLayerDefinition.depths[p1]) / 255.0f, 1.0f); // whats the magic formular here ???
				a2 = glm::min(static_cast<float>(LiquidLayerDefinition.depths[p2]) / 255.0f, 1.0f);
				a3 = glm::min(static_cast<float>(LiquidLayerDefinition.depths[p3]) / 255.0f, 1.0f);
				a4 = glm::min(static_cast<float>(LiquidLayerDefinition.depths[p4]) / 255.0f, 1.0f);
			}

			// Skip hidden water tile
			if (false == LiquidLayerDefinition.renderTiles.empty())
			{
				if (false == LiquidLayerDefinition.renderTiles[tx + ty * LiquidLayerDefinition.TileWidth])
				{
					continue;
				}
			}

			// Vertices
			verticesBuffer.push_back(glm::vec3(C_UnitSize * static_cast<float>(x0), h1, (LiquidLayerDefinition.NeedInvertY ? -1.0f : 1.0f) * (C_UnitSize * static_cast<float>(y0))));
			textureCoordBuffer.push_back(glm::vec3(t1.first, t1.second, a1));

			verticesBuffer.push_back(glm::vec3(C_UnitSize * static_cast<float>(x0), h2, (LiquidLayerDefinition.NeedInvertY ? -1.0f : 1.0f) * (C_UnitSize + C_UnitSize * static_cast<float>(y0))));
			textureCoordBuffer.push_back(glm::vec3(t2.first, t2.second, a2));

			verticesBuffer.push_back(glm::vec3(C_UnitSize + C_UnitSize * static_cast<float>(x0), h4, (LiquidLayerDefinition.NeedInvertY ? -1.0f : 1.0f) * (C_UnitSize * static_cast<float>(y0))));
			textureCoordBuffer.push_back(glm::vec3(t4.first, t4.second, a4));

			verticesBuffer.push_back(glm::vec3(C_UnitSize + C_UnitSize * static_cast<float>(x0), h3, (LiquidLayerDefinition.NeedInvertY ? -1.0f : 1.0f) * (C_UnitSize + C_UnitSize * static_cast<float>(y0))));
			textureCoordBuffer.push_back(glm::vec3(t3.first, t3.second, a3));

			indicesBuffer.push_back(indexCntr + 2);
			indicesBuffer.push_back(indexCntr + 1);
			indicesBuffer.push_back(indexCntr + 0);
			indicesBuffer.push_back(indexCntr + 3);
			indicesBuffer.push_back(indexCntr + 1);
			indicesBuffer.push_back(indexCntr + 2);
			indexCntr += 4;
		}
	}

	std::shared_ptr<IGeometry> geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
	geometry->AddVertexBuffer(BufferBinding("POSITION", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(verticesBuffer));
	geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(textureCoordBuffer));
	geometry->SetIndexBuffer(m_RenderDevice.GetObjectsFactory().CreateIndexBuffer(indicesBuffer));

	return geometry;
}
