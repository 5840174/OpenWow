#pragma once

// FORWARD BEGIN
class CWMO;
class CWMO_Base_Instance;
// FORWARD END

#include "WMO_Part_Light.h"

class ZN_API CWMOLight
	: public CLight
{
public:
	CWMOLight(const IBaseManager& BaseManager, const WMO_Part_Light& WMOLight);
	virtual ~CWMOLight();

	void Calculate(const CWMO_Base_Instance& WMOInstance, uint32 GlobalTime);

	glm::vec3 GetLightPosition() const;

private:
	const WMO_Part_Light m_WMOLight;
};