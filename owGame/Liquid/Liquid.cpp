#include "stdafx.h"

// General
#include "Liquid.h"


CLiquid::CLiquid(IRenderDevice& RenderDevice, uint32 x, uint32 y)
	: m_TilesX(x)
	, m_TilesY(y)
	, m_MinHeight(Math::MaxFloat)
	, m_MaxHeight(Math::MinFloat)
	, m_RenderDevice(RenderDevice)
{}

CLiquid::~CLiquid()
{
}



//
// ISceneNodeProvider
//
void CLiquid::CreateInsances(const std::shared_ptr<ISceneNode>& Parent) const
{
	_ASSERT(Parent != nullptr);

	auto modelsComponent = Parent->GetComponentT<IModelComponent>();
	for (const auto& it : m_WaterLayers)
		modelsComponent->AddModel(it);
}

float CLiquid::getMinHeight() const
{
	return m_MinHeight;
}

float CLiquid::getMaxHeight() const
{
	return m_MaxHeight;
}



void CLiquid::createLayers(const DBC_LiquidTypeRecord* LiquidTypeRecord, const std::shared_ptr<IByteBuffer>& Bytes, bool NeedInvertY)
{
	const SLiquidVertex* map = (const SLiquidVertex*)(Bytes->getDataFromCurrent());
	const SLiquidFlag* flags = (const SLiquidFlag*)(Bytes->getDataFromCurrent() + ((m_TilesX + 1) * (m_TilesY + 1)) * sizeof(SLiquidVertex));

	SLiquidLayerDefinition layerDefinition;
	layerDefinition.NeedInvertY = NeedInvertY;
	layerDefinition.TileStartX = 0;
	layerDefinition.TileStartY = 0;
	layerDefinition.TileWidth = m_TilesX;
	layerDefinition.TileHeight = m_TilesY;
	
    for (uint32 y = 0; y < m_TilesY + 1; y++)
    {
        for (uint32 x = 0; x < m_TilesX + 1; x++)
        {
			uint32 p = x + y * (m_TilesX + 1);
			bool isNeedDraw = ! ((flags[p].liquid & 0x08) != 0);

			layerDefinition.renderTiles.push_back(isNeedDraw);


			if (LiquidTypeRecord->Get_Type() == DBC_LIQUIDTYPE_Type::water)
			{
				layerDefinition.depths.push_back(map[p].waterVert.depth);
				layerDefinition.heights.push_back(map[p].waterVert.height);
			}
			else if (LiquidTypeRecord->Get_Type() == DBC_LIQUIDTYPE_Type::ocean)
			{
				layerDefinition.depths.push_back(map[p].oceanVert.depth);
			}
			else // magma and slime
			{
				layerDefinition.textureCoords.push_back(std::make_pair(static_cast<float>(map[p].magmaVert.s) / 255.0f, static_cast<float>(map[p].magmaVert.t) / 255.0f));
				layerDefinition.heights.push_back(map[p].magmaVert.height);
			}
        }
    }

	std::shared_ptr<CLiquidModel> liquidModel = MakeShared(CLiquidModel, m_RenderDevice);
	liquidModel->Initialize(layerDefinition, LiquidTypeRecord);
	m_WaterLayers.push_back(liquidModel);
}
