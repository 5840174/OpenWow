#include "stdafx.h"

// General
#include "WMO_Liquid.h"

CWMO_Liquid::CWMO_Liquid(IRenderDevice& RenderDevice, const CWMO& WMOObject, const WMO_Group& WMOGroupObject, const std::shared_ptr<IByteBuffer>& Bytes, const SWMO_Group_MLIQDef& LiquidHeader)
	: CLiquid(RenderDevice, LiquidHeader.A, LiquidHeader.B)
	, m_WMOObject(WMOObject)
	, m_WMOGroupObject(WMOGroupObject)
	, m_LiquidHeader(LiquidHeader)
{
	ydir = -1.0f; // Magic for WMO
	createLayers(RenderDevice.GetBaseManager().GetManager<CDBCStorage>()->DBC_LiquidType()[1], Bytes);

	// m_WMOLiqiud->CreateFromWMO(buffer, m_WMOModel.m_Materials[m_LiquidHeader.materialID], m_BaseManager.GetManager<CDBCStorage>()->DBC_LiquidType()[1], m_GroupHeader.flags.IS_INDOOR);

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

CWMO_Liquid::~CWMO_Liquid()
{

}
