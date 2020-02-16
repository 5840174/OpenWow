#include "stdafx.h"

// General
#include "WMO_Liquid.h"

CWMO_Liquid::CWMO_Liquid(IRenderDevice& RenderDevice, uint32 _x, uint32 _y) :
	CLiquid(RenderDevice, _x, _y)
{}

CWMO_Liquid::~CWMO_Liquid()
{
}

void CWMO_Liquid::CreateFromWMO(std::shared_ptr<IByteBuffer> f, std::shared_ptr<const WMO_Part_Material> _material, const DBC_LiquidTypeRecord* _liquidType, bool _indoor)
{
	ydir = -1.0f; // Magic for WMO
	createLayers(_liquidType, f);

	//SLiquidVertex* map = (SLiquidVertex*)(f->getDataFromCurrent());
	//SLiquidFlag* flags = (SLiquidFlag*)(f->getDataFromCurrent() + ((m_TilesX + 1) * (m_TilesY + 1)) * sizeof(SLiquidVertex));

	//Log::Green("LQQ type [%d]", flags[0].liquid & 3);

	/*if (_indoor)
	{
		m_WaterColorLight = fromRGB(_material->GetDiffuseColor());
		m_WaterColorDark = m_WaterColorLight;
	}
	else
	{
		m_WaterColorLight = _World->EnvM()->m_SkyManager->GetColor(LIGHT_COLOR_RIVER_LIGHT);
		m_WaterColorDark = _World->EnvM()->m_SkyManager->GetColor(LIGHT_COLOR_RIVER_DARK);
	}*/
}