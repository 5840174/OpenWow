#include "stdafx.h"

// General
#include "LiquidLayer.h"

CLiquidLayer::CLiquidLayer(IRenderDevice& RenderDevice, std::shared_ptr<IModel> Model)
	: ModelProxie(Model)
	, m_RenderDevice(RenderDevice)
{
	m_Material = std::make_shared<LiquidMaterial>(m_RenderDevice);
}

CLiquidLayer::~CLiquidLayer()
{
}



//
// IModel
//
bool CLiquidLayer::Render(const RenderEventArgs& renderEventArgs) const
{
	uint32_t texidx = (uint32_t)(renderEventArgs.TotalTime * 1000.0f / 60.0f) % m_Textures.size();
	m_Material->SetTexture(0, m_Textures[texidx]);

	std::shared_ptr<ISkyManager> SkyManager = nullptr; // m_SkyManager.lock();

	if (SkyManager != nullptr)
	{
		m_Material->SetColorLight(SkyManager->GetColor(LightColors::LIGHT_COLOR_RIVER_LIGHT));
		m_Material->SetColorDark(SkyManager->GetColor(LightColors::LIGHT_COLOR_RIVER_DARK));
		m_Material->SetShallowAlpha(SkyManager->GetWaterShallowAlpha());
		m_Material->SetDeepAlpha(SkyManager->GetWaterDarkAlpha());
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

	char buf[256];
	for (int i = 1; i <= 30; i++)
	{
        sprintf(buf, "%s.%d.blp", baseName.c_str(), i);
		std::shared_ptr<ITexture> texture = m_RenderDevice.GetObjectsFactory().LoadTexture2D(buf);
		m_Textures.push_back(texture);
	}
}

const std::shared_ptr<IMaterial> CLiquidLayer::GetMaterial() const
{
	return m_Material;
}
