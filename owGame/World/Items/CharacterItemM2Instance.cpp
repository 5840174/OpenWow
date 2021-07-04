#include "stdafx.h"

#ifdef USE_M2_MODELS

// General
#include "CharacterItemM2Instance.h"

CCharacterItemM2Instance::CCharacterItemM2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object)
	: CM2_Base_Instance(Scene, M2Object)
{}

CCharacterItemM2Instance::~CCharacterItemM2Instance()
{}



//
// CCharacterItemM2Instance
//
void CCharacterItemM2Instance::AddVisualEffect(std::shared_ptr<CM2_Base_Instance> _visualEffect)
{
	if (GetState() == ILoadable::ELoadableState::Loaded)
	{
		m_VisualEffects.push_back(_visualEffect);
	}
	else
	{

	}
}



//
// ILoadable
//
bool CCharacterItemM2Instance::Load()
{
	return __super::Load();
}

#endif