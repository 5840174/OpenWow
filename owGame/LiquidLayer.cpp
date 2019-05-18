#include "stdafx.h"

// General
#include "LiquidLayer.h"

CLiquidLayer::CLiquidLayer(std::shared_ptr<IMesh> _mesh)
	: MeshWrapper(_mesh)
	, m_SkyManager(GetManager<ISkyManager>())
	, m_QualitySettings(GetSettingsGroup<CGroupQuality>())
{
	m_Material = std::make_shared<LiquidMaterial>();

	SetMaterial(m_Material);
}

CLiquidLayer::~CLiquidLayer()
{
}

bool CLiquidLayer::Render(const RenderEventArgs* renderEventArgs, std::shared_ptr<ConstantBuffer> perObject, UINT indexStartLocation, UINT indexCnt, UINT vertexStartLocation, UINT vertexCnt)
{
	uint32_t texidx = (uint32_t)(EngineTime::GetTotalTime() * 1000.0f / 60.0f) % m_Textures.size();
	m_Material->SetTexture(0, m_Textures[texidx]);

	std::shared_ptr<ISkyManager> SkyManager = m_SkyManager.lock();

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

	return MeshWrapper::Render(renderEventArgs, perObject, indexStartLocation, indexCnt, vertexStartLocation, vertexCnt);
}

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
        fail1();
    }

	char buf[256];
	for (int i = 1; i <= 30; i++)
	{
        sprintf(buf, "%s.%d.blp", baseName.c_str(), i);
		std::shared_ptr<Texture> texture = _RenderDevice->CreateTexture2D(buf);
		m_Textures.push_back(texture);
	}
}
