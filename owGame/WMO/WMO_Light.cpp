#include "stdafx.h"

// General
#include "WMO_Light.h"

CWMOLight::CWMOLight(const IBaseManager& BaseManager, const WMO_Part_Light& WMOLight)
	: CLight(BaseManager)
	, m_WMOLight(WMOLight)
{
	SetEnabled(true);
}

CWMOLight::~CWMOLight()
{}

void CWMOLight::Calculate(const CWMO_Base_Instance& WMOInstance, uint32 GlobalTime)
{
	if (m_WMOLight.GetProto().type == SWMO_MOLT::ELightType::DIRECT_LGT)
	{
		SetType(ELightType::Directional);
	}
	else if (m_WMOLight.GetProto().type == SWMO_MOLT::ELightType::OMNI_LGT)
	{
		SetType(ELightType::Point);
	}
	else
		throw CException("CWMOLight::Calculate: Unknown light type.");


	ColorRGBA color = m_WMOLight.GetProto().GetColor();
	SetColor(ColorRGB(color.r, color.g, color.b));

	SetRange(m_WMOLight.GetProto().attenEnd * 1.0f);
}

glm::vec3 CWMOLight::GetLightPosition() const
{
	return m_WMOLight.GetPosition();
}
