#pragma once

#include "M2_Part_Light.h"

class ZN_API CM2Light
	: public CLight
{
public:
	CM2Light(const IBaseManager& BaseManager, const CM2_Part_Light& M2Light);
	virtual ~CM2Light();
	
	void Calculate(const CM2_Base_Instance& M2Instance, uint32 GlobalTime);

	glm::vec3 GetLightPosition() const;

private:
	CM2_Part_Light m_M2Light;
};