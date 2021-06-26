#pragma once

#include "LiquidMaterial.h"

struct SLiquidLayerDefinition
{
	SLiquidLayerDefinition()
		: NeedInvertY(false)
	{}

	//const DBC_LiquidTypeRecord* LiquidType;
	//uint16                      VertexFormat;
	bool                        NeedInvertY;

	//float                       MinHeightLevel;
	//float                       MaxHeightLevel;

	uint8                       TileStartX;
	uint8                       TileStartY;
	uint8                       TileWidth;
	uint8                       TileHeight;

	bool                        hasmask;
	uint8                       mask[8];
	
	std::vector<float>          heights;
	std::vector<uint8>          depths;
	std::vector<std::pair<float, float>> textureCoords;

	std::vector<bool>           renderTiles;
};



class ZN_API CLiquidModel 
	: public ModelProxie
{
public:
	CLiquidModel(IRenderDevice& RenderDevice);
	virtual ~CLiquidModel();

	void Initialize(const SLiquidLayerDefinition& LiquidLayerDefinition, const DBC_LiquidTypeRecord* LiquidTypeRecord);
	
	std::shared_ptr<IMaterial> GetMaterial() const;

	// IModel
	bool Render(const ShaderMap& Shaders) const override;

private:
	std::shared_ptr<IGeometry> CreateGeometryByDefinition(const SLiquidLayerDefinition& LiquidLayerDefinition);

private:
	ISkyManager* m_SkyManager;
	std::shared_ptr<CLiquidMaterial> m_Material;
	DBC_LIQUIDTYPE_Type m_LiquidType;

private:
	IRenderDevice& m_RenderDevice;
};