#pragma once

#include "LiquidMaterial.h"

#include "DBC\\Core\DBC__Storage.h"

class ZN_API CLiquidLayer 
	: public ModelProxie
{
public:
	CLiquidLayer(IRenderDevice& RenderDevice, std::shared_ptr<IModel> Model);
	virtual ~CLiquidLayer();

	// IModel
	bool Render(const RenderEventArgs& renderEventArgs) const override;

public:
	std::shared_ptr<const DBC_LiquidTypeRecord> LiquidType;

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

	const std::shared_ptr<IMaterial> GetMaterial() const;

private:
	std::vector<std::shared_ptr<ITexture>>    m_Textures;
	std::shared_ptr<LiquidMaterial> m_Material;

private:
	IRenderDevice& m_RenderDevice;
};