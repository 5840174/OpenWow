#include "stdafx.h"

// General
#include "LiquidMaterial.h"

CLiquidMaterial::CLiquidMaterial(IRenderDevice& RenderDevice)
	: MaterialProxieT(RenderDevice.GetObjectsFactory().CreateMaterial("CLiquidMaterial"))
	, m_RenderDevice(RenderDevice)
{}

CLiquidMaterial::~CLiquidMaterial()
{}



//
// CLiquidMaterial
//
void CLiquidMaterial::SetShallowAlpha(float value)
{
	MaterialData().gShallowAlpha = value;
}

void CLiquidMaterial::SetDeepAlpha(float value)
{
	MaterialData().gDeepAlpha = value;
}

void CLiquidMaterial::SetColorLight(ColorRGB value)
{
	MaterialData().gColorLight = value;
}

void CLiquidMaterial::SetColorDark(ColorRGB value)
{
	MaterialData().gColorDark = value;
}

void CLiquidMaterial::InitializeTextures(const DBC_LiquidTypeRecord* LiquidTypeRecord)
{
	DBC_LIQUIDTYPE_Type liquidType = LiquidTypeRecord->Get_Type();
	MaterialData().gLiquidType = (uint32)liquidType;

	std::string liquidTextureName = LiquidTypeRecord->Get_Texture(0);
	if (liquidTextureName.empty())
		throw CException("CLiquidMaterial: Liquid texture name for '%d' is empty.", (uint32)liquidType);

	/*std::string baseName;

	if (LiquidType == DBC_LIQUIDTYPE_Type::water)
	{
		baseName = "XTextures\\river\\lake_a";
	}
	else if (LiquidType == DBC_LIQUIDTYPE_Type::ocean)
	{
		baseName = "XTextures\\ocean\\ocean_h";
	}
	else if (LiquidType == DBC_LIQUIDTYPE_Type::magma)
	{
		baseName = "XTextures\\lava\\lava";
	}
	else if (LiquidType == DBC_LIQUIDTYPE_Type::slime)
	{
		baseName = "XTextures\\slime\\slime";
	}
	else
	{
		throw CException("Unknown liquid type.");
	}*/

	for (int i = 1; i <= 30; i++)
	{
		char texturePath[MAX_PATH];
		if (-1 == sprintf_s(texturePath, liquidTextureName.c_str(), i))
			throw CException("CLiquidMaterial: Unable to sprintf texture filename for '%d'.", (uint32)liquidType);

		//std::string textureFilename = liquidTextureName + "." + std::to_string(i) + ".blp";

		std::shared_ptr<ITexture> texture = m_RenderDevice.GetBaseManager().GetManager<IznTexturesFactory>()->LoadTexture2D(texturePath);
		m_Textures.push_back(texture);
	}
}

void CLiquidMaterial::Update()
{
	uint32_t texidx = (uint32_t)(GetTickCount() / 60.0f) % m_Textures.size();
	SetTexture(0, m_Textures[texidx]);
}
