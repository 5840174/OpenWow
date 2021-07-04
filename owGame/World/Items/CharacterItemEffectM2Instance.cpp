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

const CCharacterItemM2Instance & CCharacterItemEffectM2Instance::GetCharacterItemInstance() const
{
	auto parentIsCharacter = GetParent();
	if (parentIsCharacter == nullptr)
		throw CException("CCharacterItemEffectM2Instance::GetCharacterItemInstance: M2Parent must exists.");

	if (auto loadable = std::dynamic_pointer_cast<ILoadable>(parentIsCharacter))
		if (loadable->GetState() != ILoadable::ELoadableState::Loaded)
			throw CException("CCharacterItemEffectM2Instance::GetCharacterItemInstance: M2Parent CharacterItem isn't loaded.");

	return dynamic_cast<CCharacterItemM2Instance&>(*parentIsCharacter);
}

const CCharacterItem & CCharacterItemEffectM2Instance::GetCharacterItem() const
{
	if (auto loadable = dynamic_cast<const ILoadable*>(&m_CharacterItem))
		if (loadable->GetState() != ILoadable::ELoadableState::Loaded)
			throw CException("CCharacterItemM2Instance::GetCharacter: M2Parent character isn't loaded.");

	return m_CharacterItem;
}



//
// ILoadable
//
bool CCharacterItemEffectM2Instance::Load()
{
	bool loadResult =  __super::Load();
	if (false == loadResult)
		return false;

	return loadResult;
}

#endif