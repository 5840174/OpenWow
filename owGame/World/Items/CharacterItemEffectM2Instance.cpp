#include "stdafx.h"

#ifdef USE_M2_MODELS

// Include
#include "CharacterItem.h"
#include "CharacterItemM2Instance.h"

// General
#include "CharacterItemEffectM2Instance.h"

CCharacterItemEffectM2Instance::CCharacterItemEffectM2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object, const CCharacterItem& CharacterItem)
	: CM2_Base_Instance(Scene, M2Object)
	, m_CharacterItem(CharacterItem)
{}

CCharacterItemEffectM2Instance::~CCharacterItemEffectM2Instance()
{}

const CCharacterItem & CCharacterItemEffectM2Instance::GetCharacterItem() const
{
	if (auto loadable = dynamic_cast<const ILoadable*>(&m_CharacterItem))
	{
		auto state = loadable->GetState();
		if (state != ILoadable::ELoadableState::Loaded)
			throw CException("CCharacterItemM2Instance::GetCharacterItem: CharacterItem character isn't loaded. State = '%d'.", state);
	}
	return m_CharacterItem;
}



//
// ILoadable
//
void CCharacterItemEffectM2Instance::OnLoaded()
{
}

#endif