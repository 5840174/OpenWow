#pragma once

#ifdef USE_M2_MODELS

#include "M2/M2_Base_Instance.h"

class CItem_M2Instance 
	: public CM2_Base_Instance
{
public:
	CItem_M2Instance(const std::shared_ptr<CM2>& M2Object);
    virtual ~CItem_M2Instance();

	void AddVisualEffect(std::shared_ptr<CM2_Base_Instance> _visualEffect);

private:
	std::vector<std::shared_ptr<CM2_Base_Instance>> m_VisualEffects;
};

#endif