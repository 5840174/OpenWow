#include "stdafx.h"

#ifdef USE_WMO_MODELS

// General
#include "WMOGroup_Part_Liquid.h"

CWMOGroup_Part_Liquid::CWMOGroup_Part_Liquid(IRenderDevice& RenderDevice, const CWMO& WMOObject, const CWMOGroup& WMOGroupObject, const std::shared_ptr<IByteBuffer>& Bytes, const SWMOGroup_MLIQ& LiquidHeader, const DBC_LiquidTypeRecord* LiquidType)
	: CLiquid(RenderDevice, LiquidHeader.A, LiquidHeader.B)
	, m_WMOObject(WMOObject)
	, m_WMOGroupObject(WMOGroupObject)
	, m_LiquidHeader(LiquidHeader)
{
	m_MinHeight = Math::MinFloat;
	m_MaxHeight = Math::MaxFloat;

	createLayers(LiquidType, Bytes, true);

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

CWMOGroup_Part_Liquid::~CWMOGroup_Part_Liquid()
{

}

#endif
