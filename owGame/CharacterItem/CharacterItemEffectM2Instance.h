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
	CCharacterItemEffectM2Instance(IScene& Scene, CWorldClient& WorldClient, const std::shared_ptr<CM2>& M2Object, const CCharacterItem& CharacterItem);
    virtual ~CCharacterItemEffectM2Instance();

	const CCharacterItem& GetCharacterItem() const;

	// ILoadable
	void OnAfterLoad() override;

private:
	const CCharacterItem& m_CharacterItem;
};

#endif