#pragma once

#ifdef USE_M2_MODELS

#include "../../M2/M2_Base_Instance.h"

// FORWARD BEGIN
class CCharacterItemM2Instance;
// FORWARD END

class CCharacterItemEffectM2Instance
	: public CM2_Base_Instance
{
public:
	CCharacterItemEffectM2Instance(IScene& Scene, const std::shared_ptr<CM2>& M2Object, const CCharacterItem& CharacterItem);
    virtual ~CCharacterItemEffectM2Instance();

	const CCharacterItemM2Instance& GetCharacterItemInstance() const;
	const CCharacterItem& GetCharacterItem() const;

	// ILoadable
	void OnLoaded() override;

private:
	const CCharacterItem& m_CharacterItem;
};

#endif