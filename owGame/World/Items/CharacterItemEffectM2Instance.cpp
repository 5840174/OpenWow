#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "CharacterItemEffectM2Instance.h"

CCharacterItemEffectM2Instance::CCharacterItemEffectM2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(Scene, M2Object)
{}

CCharacterItemEffectM2Instance::~CCharacterItemEffectM2Instance()
{}

void CCharacterItemEffectM2Instance::AddVisualEffect(std::shared_ptr<CM2_Base_Instance> _visualEffect)
{
	AddChild(_visualEffect);
}

#endif