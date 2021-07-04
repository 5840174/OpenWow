#pragma once

#ifdef USE_M2_MODELS

#include "../../M2/M2_Base_Instance.h"

class CCharacterItemM2Instance
	: public CM2_Base_Instance
{
public:
	CCharacterItemM2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object);
    virtual ~CCharacterItemM2Instance();

	// CCharacterItemM2Instance
	void AddVisualEffect(std::shared_ptr<CM2_Base_Instance> _visualEffect);

	// ILoadable
	bool Load() override;

private:
	std::vector<std::shared_ptr<CM2_Base_Instance>> m_VisualEffects;
};

#endif