#pragma once

#include "LiquidMaterial.h"

#include "DBC/DBC__Storage.h"

class ZN_API CLiquidLayer 
	: public ModelProxie
{
public:
	CLiquidLayer(IRenderDevice& RenderDevice);
	virtual ~CLiquidLayer();

	std::shared_ptr<IGeometry> CreateGeometry(float YDir);
	std::shared_ptr<IMaterial> GetMaterial() const;

	// IModel
	bool Render(const RenderEventArgs& renderEventArgs) const override;

public:
	const DBC_LiquidTypeRecord* LiquidType;

	float MinHeightLevel;
	float MaxHeightLevel;

	uint8 x;
	uint8 y;
	uint8 Width;
	uint8 Height;

	bool hasmask;
	uint8 mask[8];

	std::vector<float> heights;
	std::vector<uint8> depths;
	std::vector<std::pair<float, float>> textureCoords;

	std::vector<bool> renderTiles;

public:
	// Render
	void InitTextures(DBC_LIQUIDTYPE_Type::List _liquidType);

	

private:
	ISkyManager* m_SkyManager;
	std::vector<std::shared_ptr<ITexture>>    m_Textures;
	std::shared_ptr<LiquidMaterial> m_Material;

private:
	IRenderDevice& m_RenderDevice;
};